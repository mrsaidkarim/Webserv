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

void HttpResponse::cgi() const{
    // get env
    cout << "here\n";
    string http_response_header =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Connection: keep-alive\r\n"
        "\r\n"
        "<h1>Found CGI</h1>";
    if (send(request->get_client_socket(), http_response_header.c_str(), http_response_header.size(), 0)) {
        // perror("send failed in send_response()");
        // request->set_is_complete(true);
    }
    request->set_is_complete(true);

    char **env = header_to_env();
    print_env(env);

    cout << request->get_server().get_locations()[index_location].get_path_cgi("py").c_str() << "\n";
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

        execve(args[0], args, env);
        // char** char_pointer_array = new char*[2]; // 2 elements: one for the string, one for the NULL terminator

        // // Allocate memory for the string and copy it
        // char_pointer_array[0] = new char[request->get_file_path().size() + 1]; // +1 for the null terminator
        // strcpy(char_pointer_array[0], request->get_file_path().c_str());

        // // Null-terminate the array
        // char_pointer_array[1] = NULL;
        // // close(kq);
        // execve(request->get_server().get_locations()[index_location].get_path_cgi("py").c_str(), char_pointer_array, env);
    // const string python_path = "/usr/bin/python3"; // Path to the Python interpreter
    // const string script_path = request->get_file_path(); // Path to the Python script

    // char **argv = new char *[3]; // 3 elements: python path, script path, NULL terminator
    // argv[0] = new char[python_path.size() + 1];
    // strcpy(argv[0], python_path.c_str());
    // argv[1] = new char[script_path.size() + 1];
    // strcpy(argv[1], script_path.c_str());
    // argv[2] = NULL; // Null-terminate the array

    // Execute the CGI script
    // execve(python_path.c_str(), argv, env);
        cerr << "execve failed\n";
        exit(1);
    
    } else {
        //parent
        exit_status = -1;
        waitpid(pid, &exit_status, 0);
        cout << " the exist status for the child is :" << exit_status << "\n";
    }
    // we need path of file (stdin) and dup it with 0
    // create output file and dup it with 1
    // create child process && exec the script (cgi)
    // return responce back to the client using get method
}