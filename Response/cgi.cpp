#include "HttpResponse.hpp"
#include <sys/_types/_pid_t.h>
#include <sys/fcntl.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <unistd.h>

char** HttpResponse::header_to_env() const{
    char** env = new char*[request->get_header().size() + 1];
    // try catch for new failed
    int i = 0;
    map<string, string>::const_iterator it = request->get_header().begin();
    while (it != request->get_header().end()) {
        string key = it->first;
        string value = it->second;
        transform(key.begin(), key.end(), key.begin(), toupper);
        std::replace(key.begin(), key.end(), '-', '_');
        string sum = "HTTP_" + key + "=" + value; // here nigro sir 9alb ojibo
        env[i] = new char[sum.size() + 1];
        copy(sum.begin(), sum.end(), env[i]);
        env[i][sum.size()] = 0;
        it++;
        i++;
    }

    env[i] = NULL;
    return (env);
}


bool HttpResponse::is_cgi() const{
    // to remove
    if (request->get_is_cgi())
        return (true);
    cout << "cgi file path: " << request->get_file_path() << "\n";
    size_t pos = request->get_file_path().rfind(".");
    if (pos == string::npos)
        return (false);
    string extension = request->get_file_path().substr(pos + 1);
    cout << extension << "\n";
    if (extension != "py" && extension != "php" && extension != "js")
        return (false);
    const string path = request->get_server().get_locations()[index_location].get_path_cgi(extension);
    if (path.empty()) {
        request->set_file_path(FORBIDDEN);
        return (false);
    }
    return (true); // check extension is matched with cgi
}

void HttpResponse::print_env(char **env) const {
    if (!env) {
        cerr << "env if empty!\n";
        return;
    }

    int i = 0;
    cout << "--------------------env--------------------\n";
    while (env[i]) {
        cout << "[" << env[i] << "]\n";
        i++;
    }
    cout << "-------------------------------------------\n";
}

void HttpResponse::handle_timeout(pid_t pid, const string& file_path) const {
    struct kevent change;

    // Monitor the child process exit event
    EV_SET(&change, pid, EVFILT_PROC, EV_ADD | EV_ENABLE, NOTE_EXIT, 0, nullptr);
    if (kevent(kq, &change, 1, nullptr, 0, nullptr) == -1) {
        cerr << "Failed to monitor child process\n";
        kill(pid, SIGKILL);
        request->set_file_path(INTERNAL_SERVER_ERROR);
        send_response();
        return;
    }

    // Add timeout monitoring (this is a timer that triggers if the child takes too long)
    struct kevent timeout_event;
    EV_SET(&timeout_event, pid, EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, CGI_TIMEOUT, nullptr);  // Timeout in milliseconds
    if (kevent(kq, &timeout_event, 1, nullptr, 0, nullptr) == -1) {
        cerr << BOLD_RED << "Failed to add timeout event\n" << RESET;
        kill(pid, SIGKILL);
        request->set_file_path(INTERNAL_SERVER_ERROR);
        send_response();
        return;
    }

    // Now handle events asynchronously
    struct kevent event;
    int num_events = kevent(kq, nullptr, 0, &event, 1, nullptr);
    while (num_events > 0) {
        if (event.filter == EVFILT_PROC) {
            // Child process exit event
            pid_t child_pid = event.ident; // The pid of the child that triggered the event
            int status = event.data; // The exit status data

            if (WIFEXITED(status)) {
                int exit_status = WEXITSTATUS(status);
                cout << "Child process " << child_pid << " exited with status: " << exit_status << "\n";

                if (exit_status == 0) {
                    request->set_file_path(file_path); // Successful CGI execution
                } else {
                    cerr << "CGI process exited with error status: " << exit_status << "\n";
                    request->set_file_path(INTERNAL_SERVER_ERROR);
                }
            } else if (WIFSIGNALED(status)) {
                int signal_num = WTERMSIG(status);
                cerr << "Child process " << child_pid << " was terminated by signal: " << signal_num << "\n";
                request->set_file_path(INTERNAL_SERVER_ERROR);
            } else {
                cerr << "Unexpected child process exit status\n";
                request->set_file_path(INTERNAL_SERVER_ERROR);
            }

            // Mark request as complete and send response
            // request->set_is_complete(true);
            request->set_is_cgi(false);
            send_response();
            EV_SET(&change, pid, EVFILT_PROC, EV_DELETE, 0, 0, nullptr);
            if (kevent(kq, &change, 1, nullptr, 0, nullptr) == -1) {
                cerr << "Failed to remove child process monitoring\n";
            }

            // Remove the timeout event
            EV_SET(&timeout_event, pid, EVFILT_TIMER, EV_DELETE, 0, 0, nullptr);
            if (kevent(kq, &timeout_event, 1, nullptr, 0, nullptr) == -1) {
                cerr << "Failed to remove timeout event\n";
            }
            return;  // Exit since we've handled the exit event
        } else if (event.filter == EVFILT_TIMER) {
            // Timeout event triggered (if CGI process takes too long)
            cerr << "CGI process exceeded timeout\n";
            kill(pid, SIGKILL);  // Kill the child process if it's still running
            request->set_file_path(REQUEST_TIMEOUT);
            // request->set_is_complete(true);
            send_response();
            EV_SET(&change, pid, EVFILT_PROC, EV_DELETE, 0, 0, nullptr);
            if (kevent(kq, &change, 1, nullptr, 0, nullptr) == -1) {
                cerr << "Failed to remove child process monitoring\n";
            }

            // Remove the timeout event
            EV_SET(&timeout_event, pid, EVFILT_TIMER, EV_DELETE, 0, 0, nullptr);
            if (kevent(kq, &timeout_event, 1, nullptr, 0, nullptr) == -1) {
                cerr << "Failed to remove timeout event\n";
            }
            return;  // Exit after handling the timeout
        }

        // Re-check the kevent queue for other events
        num_events = kevent(kq, nullptr, 0, &event, 1, nullptr);
    }
    // int exit_status = -1;
    // int rr;
    // int i = 0;
    // while (i < 2000000) {
    //     rr = waitpid(pid, &exit_status, WNOHANG);
    //     if (rr > 0)
    //         break;
    //     i++;
    // }
    // kill(pid, SIGKILL);
    // request->set_is_cgi(false);
    // if (i == 2000000)
    //     request->set_file_path(REQUEST_TIMEOUT);
    // else if ((exit_status) == 0)
    //     request->set_file_path(file_path);
    // else
    //     request->set_file_path(INTERNAL_SERVER_ERROR);
    // send_response();
    // cout << " the exist status for the child is :" << exit_status << "\n";
    cout << "here >>>>>>>>>> 4\n";
}


void HttpResponse::cgi() const{
    // get env
    cout << "here\n";
    // string http_response_header =
    //     "HTTP/1.1 200 OK\r\n"
    //     "Content-Type: text/html\r\n"
    //     "Connection: keep-alive\r\n"
    //     "\r\n"
    //     "<h1>Found CGI</h1>";
    // if (send(request->get_client_socket(), http_response_header.c_str(), http_response_header.size(), 0)) {
    //     // perror("send failed in send_response()");
    //     // request->set_is_complete(true);
    // }
    // request->set_is_complete(true);

    // std::ifstream file(request->get_cgi_path_post().c_str());

    // if (!file) {
    //     cerr << "Error: Unable to open file " << '\n';
    // }

    // cout << BG_CYAN<< "content file is : >> " <<file.rdbuf() << "\n" << RESET; // Write the file content to stdout

    // file.close();

    char **env = header_to_env();
    print_env(env);

    // cout << request->get_server().get_locations()[index_location].get_path_cgi("py").c_str() << "\n";

    string file_path = generate_file_name();
    request->set_is_unlink_file_path(true);
    cout << "output file for cgi: " << file_path << "\n";
    
    pid_t pid = fork();
    int exit_status;

    if (pid < 0) {
        cerr << "failed fork"; // try catch
        request->set_is_complete(true);
        return;
    } else if (pid == 0) {
        //child
        close(kq);
        size_t pos = request->get_file_path().rfind(".");
        string extension = request->get_file_path().substr(pos + 1);

        // int fd_write = open(file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        int fd_write = open(file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_write < 0) {
            cerr << "can't open file in child\n";
            exit(1);
        }
        char *args[3];
        if (request->get_method() == "POST") {
            request->get_file_stream()->close();
            args[0] = const_cast<char *>("/usr/bin/python3");
            args[1] = const_cast<char *>(CGI_POST_SCRIPT);
            args[2] = NULL;

            int fd_read = open(request->get_cgi_path_post().c_str(), O_RDWR);
            if (dup2(fd_read, 0) < 0) {
                cerr << "2) dup2 failed in child\n";
                close(fd_write);
                exit(1);
            }
        } else {
            cout  << "here >>> " << index_location << "\n";
            args[0] = const_cast<char *>(request->get_server().get_locations()[index_location].get_path_cgi(extension).c_str());
            args[1] = const_cast<char *>(request->get_file_path().c_str());
            args[2] = NULL;
        }
        if (dup2(fd_write, 1) < 0) {
            cerr << "1) dup2 failed in child\n";
            close(fd_write);
            exit(1);
        }
        execve(args[0], args, env);
        cerr << BOLD_RED << "execve failed\n" << RESET;
        close(fd_write);
        exit(1);
    
    } else {
        // handle_timeout(pid, file_path);
        webserv->handle_timeout(pid, file_path, this);
        // //parent
        // exit_status = -1;
        // waitpid(pid, &exit_status, 0);
        // request->set_is_cgi(false);
        // if ((exit_status) == 0)
        //     request->set_file_path(file_path);
        // else
        //     request->set_file_path(INTERNAL_SERVER_ERROR);
        // send_response();
        // cout << " the exist status for the child is :" << exit_status << "\n";
    }
    // we need path of file (stdin) and dup it with 0
    // create output file and dup it with 1
    // create child process && exec the script (cgi)
    // return responce back to the client using get method
}