#include "HttpResponse.hpp"
#include <sys/_types/_pid_t.h>
#include <sys/fcntl.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <unistd.h>

// char** HttpResponse::header_to_env() const{
//     char** env = new char*[request->get_header().size() + 1];
//     // try catch for new failed
//     int i = 0;
//     map<string, string>::const_iterator it = request->get_header().begin();
//     while (it != request->get_header().end()) {
//         string key = it->first;
//         string value = it->second;
//         transform(key.begin(), key.end(), key.begin(), toupper);
//         std::replace(key.begin(), key.end(), '-', '_');
//         string sum = "HTTP_" + key + "=" + value; // here nigro sir 9alb ojibo
//         env[i] = new char[sum.size() + 1];
//         copy(sum.begin(), sum.end(), env[i]);
//         env[i][sum.size()] = 0;
//         it++;
//         i++;
//     }

//     env[i] = NULL;
//     return (env);
// }

char** HttpResponse::header_to_env() const {
    // Vector to store environment variables as strings
    vector<string> env_vars;
    string script_name;
    for (size_t i = 0; i < request->get_url().size(); i++) {
        script_name += "/" + request->get_url()[i];
    }

    // Add additional fields first
    env_vars.push_back("QUERY_STRING=" + request->get_query());
    env_vars.push_back("PATH_INFO=" + request->get_path_info());
    env_vars.push_back("SCRIPT_NAME=" + script_name); // request->get_url()
    env_vars.push_back("SERVER_NAME=webserver zsz v1.1");
    env_vars.push_back("REQUEST_METHOD=" + request->get_method());

    map<string, string>::const_iterator it = request->get_header().begin();
    while (it != request->get_header().end()) {
        string key = it->first;
        string value = it->second;
        transform(key.begin(), key.end(), key.begin(), toupper);
        std::replace(key.begin(), key.end(), '-', '_');
        string sum = "HTTP_" + key + "=" + value; // here nigro sir 9alb ojibo
        env_vars.push_back(sum);
        it++;
    }

    // Allocate memory for env array
    char** env = NULL;
    try {
        env = new char*[env_vars.size() + 1];
    } catch (const std::bad_alloc& e) {
        cerr  << BOLD_RED << "Memory allocation failed: " << e.what() << RESET <<endl;
        return NULL;
    }

    // Copy strings from vector to env array
    try {
        for (size_t i = 0; i < env_vars.size(); i++) {
            env[i] = new char[env_vars[i].size() + 1];
            copy(env_vars[i].begin(), env_vars[i].end(), env[i]);
            env[i][env_vars[i].size()] = '\0'; // Null-terminate each string
        }
        env[env_vars.size()] = NULL;
    } catch (const std::exception& e) {
        cerr  << BOLD_RED << "Memory allocation failed: " << e.what() << RESET <<endl;
        // Free already allocated memory
        for (size_t i = 0; i < env_vars.size(); ++i) {
            if (env[i])
                delete[] env[i];
        }
        delete[] env;
        return NULL;
    }

    return env;
}

bool HttpResponse::is_cgi() const{
    // to remove
    if (request->get_is_cgi())
        return (true);
    cerr << "cgi file path: " << request->get_file_path() << "\n";
    size_t pos = request->get_file_path().rfind(".");
    if (pos == string::npos)
        return (false);
    string extension = request->get_file_path().substr(pos + 1);
    cerr << extension << "\n";
    if (extension != "py" && extension != "php") //  && extension != "js"
        return (false);
    if (index_location == -1) {
        request->set_status_code("403");
        return (false);
    }
    const string path = request->get_server().get_locations()[index_location].get_path_cgi(extension);
    if (path.empty()) {
        request->set_status_code("403");
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

// void HttpResponse::handle_timeout(pid_t pid, const string& file_path) const {
//     struct kevent change;

//     // Monitor the child process exit event
//     EV_SET(&change, pid, EVFILT_PROC, EV_ADD | EV_ENABLE, NOTE_EXIT, 0, nullptr);
//     if (kevent(kq, &change, 1, nullptr, 0, nullptr) == -1) {
//         cerr << "Failed to monitor child process\n";
//         kill(pid, SIGKILL);
//         request->set_file_path(INTERNAL_SERVER_ERROR);
//         send_response();
//         return;
//     }

//     // Add timeout monitoring (this is a timer that triggers if the child takes too long)
//     struct kevent timeout_event;
//     EV_SET(&timeout_event, pid, EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, CGI_TIMEOUT, nullptr);  // Timeout in milliseconds
//     if (kevent(kq, &timeout_event, 1, nullptr, 0, nullptr) == -1) {
//         cerr << BOLD_RED << "Failed to add timeout event\n" << RESET;
//         kill(pid, SIGKILL);
//         request->set_file_path(INTERNAL_SERVER_ERROR);
//         send_response();
//         return;
//     }

//     // Now handle events asynchronously
//     struct kevent event;
//     int num_events = kevent(kq, nullptr, 0, &event, 1, nullptr);
//     while (num_events > 0) {
//         if (event.filter == EVFILT_PROC) {
//             // Child process exit event
//             pid_t child_pid = event.ident; // The pid of the child that triggered the event
//             int status = event.data; // The exit status data

//             if (WIFEXITED(status)) {
//                 int exit_status = WEXITSTATUS(status);
//                 cout << "Child process " << child_pid << " exited with status: " << exit_status << "\n";

//                 if (exit_status == 0) {
//                     request->set_file_path(file_path); // Successful CGI execution
//                 } else {
//                     cerr << "CGI process exited with error status: " << exit_status << "\n";
//                     request->set_file_path(INTERNAL_SERVER_ERROR);
//                 }
//             } else if (WIFSIGNALED(status)) {
//                 int signal_num = WTERMSIG(status);
//                 cerr << "Child process " << child_pid << " was terminated by signal: " << signal_num << "\n";
//                 request->set_file_path(INTERNAL_SERVER_ERROR);
//             } else {
//                 cerr << "Unexpected child process exit status\n";
//                 request->set_file_path(INTERNAL_SERVER_ERROR);
//             }

//             // Mark request as complete and send response
//             // request->set_is_complete(true);
//             request->set_is_cgi(false);
//             send_response();
//             EV_SET(&change, pid, EVFILT_PROC, EV_DELETE, 0, 0, nullptr);
//             if (kevent(kq, &change, 1, nullptr, 0, nullptr) == -1) {
//                 cerr << "Failed to remove child process monitoring\n";
//             }

//             // Remove the timeout event
//             EV_SET(&timeout_event, pid, EVFILT_TIMER, EV_DELETE, 0, 0, nullptr);
//             if (kevent(kq, &timeout_event, 1, nullptr, 0, nullptr) == -1) {
//                 cerr << "Failed to remove timeout event\n";
//             }
//             return;  // Exit since we've handled the exit event
//         } else if (event.filter == EVFILT_TIMER) {
//             // Timeout event triggered (if CGI process takes too long)
//             cerr << "CGI process exceeded timeout\n";
//             kill(pid, SIGKILL);  // Kill the child process if it's still running
//             request->set_file_path(REQUEST_TIMEOUT);
//             // request->set_is_complete(true);
//             send_response();
//             EV_SET(&change, pid, EVFILT_PROC, EV_DELETE, 0, 0, nullptr);
//             if (kevent(kq, &change, 1, nullptr, 0, nullptr) == -1) {
//                 cerr << "Failed to remove child process monitoring\n";
//             }

//             // Remove the timeout event
//             EV_SET(&timeout_event, pid, EVFILT_TIMER, EV_DELETE, 0, 0, nullptr);
//             if (kevent(kq, &timeout_event, 1, nullptr, 0, nullptr) == -1) {
//                 cerr << "Failed to remove timeout event\n";
//             }
//             return;  // Exit after handling the timeout
//         }

//         // Re-check the kevent queue for other events
//         num_events = kevent(kq, nullptr, 0, &event, 1, nullptr);
//     }
//     // int exit_status = -1;
//     // int rr;
//     // int i = 0;
//     // while (i < 2000000) {
//     //     rr = waitpid(pid, &exit_status, WNOHANG);
//     //     if (rr > 0)
//     //         break;
//     //     i++;
//     // }
//     // kill(pid, SIGKILL);
//     // request->set_is_cgi(false);
//     // if (i == 2000000)
//     //     request->set_file_path(REQUEST_TIMEOUT);
//     // else if ((exit_status) == 0)
//     //     request->set_file_path(file_path);
//     // else
//     //     request->set_file_path(INTERNAL_SERVER_ERROR);
//     // send_response();
//     // cout << " the exist status for the child is :" << exit_status << "\n";
//     cout << "here >>>>>>>>>> 4\n";
// }




void HttpResponse::cgi() const{
    // get env
    cout << BG_BLUE << "here >> " << request->get_cgi_in_process() << RESET;
    if (request->get_cgi_in_process()) {
        // waitpid()
        return;
    }

    // char **env = header_to_env();
    // print_env(env);
    string file_path = generate_file_name();
    request->set_is_cgi(false);
    request->set_cgi_in_process(true);
    request->set_was_cgi(true);
    request->set_cgi_file_path(file_path);
    request->set_is_unlink_file_path(true);
    cout << "output file for cgi: " << file_path << "\n";
    
    pid_t pid = fork();

    if (pid < 0) {
        cerr << "failed fork"; // try catch
        request->set_file_path(INTERNAL_SERVER_ERROR);
        // request->set_is_complete(true);
        return;
    } else if (pid == 0) {
        //child
        webserv->close_kq();
        // close(kq);
        char **env = header_to_env();
        if (!env) {
            exit(1);
        }
        print_env(env);
        // size_t pos = request->get_file_path().rfind(".");
        // string extension = request->get_file_path().substr(pos + 1);

        // int fd_write = open(file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        int fd_write = open(file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_write < 0) {
            cerr << "can't open file in child\n";
            delete [] env;
            exit(1);
        }
        char *args[3];
        cerr << BOLD_YELLOW << request->get_method() << "\n" << RESET;
        if (!request->get_cgi_path_post().empty()) {
            
            // location matching
            // run secript
            cout << "now we should handle this1111\n";
            if (request->get_file_stream())
                request->get_file_stream()->close();
            // args[0] = const_cast<char *>("/usr/bin/python3");
            // args[1] = const_cast<char *>(CGI_POST_SCRIPT);
            // args[2] = NULL;
            // string *script_file_path = new string(get_script_path()); // here)
            string script_file_path = get_script_path();
            size_t pos = script_file_path.rfind(".");
            string extension = script_file_path.substr(pos + 1);
            cerr << "script_file_path: " << script_file_path << "\n";
            cerr << "input_file_path: " << request->get_cgi_path_post() << "\n";
            cerr << request->get_server().get_locations()[index_location].get_path_cgi(extension) << "\n";
            args[0] = const_cast<char *>(request->get_server().get_locations()[index_location].get_path_cgi(extension).c_str());
            
            // args[0] = const_cast<char *>("/usr/bin/python3");
            args[1] = const_cast<char *>(get_script_path().c_str());
            args[2] = NULL;
            cerr << BOLD_RED << request->get_server().get_locations()[index_location].get_path_cgi(extension) << RESET << endl;
            cerr << BG_RED << "this is input file: " << request->get_cgi_path_post() << "\n" <<RESET << "\n";
            int fd_read = open(request->get_cgi_path_post().c_str(), O_RDWR);
            if (dup2(fd_read, 0) < 0) {
                cerr << "2) dup2 failed in child\n";
                close(fd_write);
                delete [] env;
                _exit(2);
            }
        } else {
            size_t pos = request->get_file_path().rfind(".");
            string extension = request->get_file_path().substr(pos + 1);
            cout  << "here >>> " << index_location << "\n";
            args[0] = const_cast<char *>(request->get_server().get_locations()[index_location].get_path_cgi(extension).c_str());
            args[1] = const_cast<char *>(request->get_file_path().c_str());
            args[2] = NULL;
        }
        if (dup2(fd_write, 1) < 0) {
            cerr << "1) dup2 failed in child\n";
            close(fd_write);
            delete [] env;
            _exit(3);
        }
        execve(args[0], args, env);
        cerr << BOLD_RED << "execve failed\n" << RESET;
        close(fd_write);
        // sleep(5);
        delete [] env;
        _exit(4);
    
    } else {
        // int exit_ = -1 ;
        // waitpid(pid, &exit_, 0);
        // cerr << "the pid of the process is : >> " << pid << "the exist status is : >> " << exit_ << "\n";
        webserv->handle_timeout(pid, file_path, this);
    }
}