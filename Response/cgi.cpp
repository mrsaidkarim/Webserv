#include "HttpResponse.hpp"
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
    cout << "cgi file path: " << request->get_file_path() << "\n";
    return (request->get_is_cgi()); // check extension is matched with cgi
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

    char **env = header_to_env();
    print_env(env);

    cout << request->get_server().get_locations()[index_location].get_path_cgi("py").c_str() << "\n";
    
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
        char *args[] = {
            const_cast<char *>(request->get_server().get_locations()[index_location].get_path_cgi("py").c_str()),
            const_cast<char *>(request->get_file_path().c_str()),
            NULL
        };
        int fd = open(file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            cerr << "can't open file in child\n";
            exit(1);
        }
        if (dup2(fd, 1) < 0) {
            cerr << "dup2 failed in child\n";
            close(fd);
            exit(1);
        }
        execve(args[0], args, env);
        cerr << "execve failed\n";
        close(fd);
        exit(1);
    
    } else {
        //parent
        exit_status = -1;
        waitpid(pid, &exit_status, 0);
        request->set_is_cgi(false);
        request->set_file_path(file_path);
        send_response();
        cout << " the exist status for the child is :" << exit_status << "\n";
    }
    // we need path of file (stdin) and dup it with 0
    // create output file and dup it with 1
    // create child process && exec the script (cgi)
    // return responce back to the client using get method
}