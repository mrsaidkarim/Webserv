/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zech-chi <zech-chi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 19:58:29 by zech-chi          #+#    #+#             */
/*   Updated: 2025/01/21 22:07:02 by zech-chi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include <sys/_types/_pid_t.h>
#include <sys/fcntl.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <unistd.h>

char** HttpResponse::header_to_env() const {
    vector<string> env_vars;
    string script_name;
    for (size_t i = 0; i < request->get_url().size(); i++) {
        script_name += "/" + request->get_url()[i];
    }

    // Add additional fields first
    env_vars.push_back("QUERY_STRING=" + request->get_query());
    env_vars.push_back("PATH_INFO=" + request->get_path_info());
    env_vars.push_back("SCRIPT_NAME=" + script_name);
    env_vars.push_back("SERVER_NAME=webserver zsz v1.1");
    env_vars.push_back("REQUEST_METHOD=" + request->get_method());

    map<string, string>::const_iterator it = request->get_header().begin();
    while (it != request->get_header().end()) {
        string key = it->first;
        string value = it->second;
        transform(key.begin(), key.end(), key.begin(), toupper);
        std::replace(key.begin(), key.end(), '-', '_');
        string sum = "HTTP_" + key + "=" + value;
        env_vars.push_back(sum);
        it++;
    }

    // Allocate memory for env array
    char** env = NULL;
    try {
        env = new char*[env_vars.size() + 1];
    } catch (const std::bad_alloc& e) {
        DEBUG_MODE && cerr << BOLD_RED << "Memory allocation failed: " << e.what() << RESET <<endl;
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
        DEBUG_MODE && cerr << BOLD_RED << "Memory allocation failed: " << e.what() << RESET <<endl;
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
    if (request->get_is_cgi())
        return (true);
    if (request->get_is_cgi_complete())
        return (false);
    DEBUG_MODE && cerr << "cgi file path: " << request->get_file_path() << "\n";
    size_t pos = request->get_file_path().rfind(".");
    if (pos == string::npos)
        return (false);
    string extension = request->get_file_path().substr(pos + 1);
    DEBUG_MODE && cerr << extension << "\n";
    if (extension != "py" && extension != "php" && extension != "sh")
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
    return (true);
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
    if (request->get_cgi_in_process()) {
        return;
    }

    string file_path = generate_file_name();
    request->set_is_cgi(false);
    request->set_cgi_in_process(true);
    request->set_was_cgi(true);
    request->set_cgi_file_path(file_path);
    request->set_is_unlink_file_path(true);
    DEBUG_MODE && cout << "output file for cgi: " << file_path << "\n";
    
    pid_t pid = fork();

    if (pid < 0) {
        DEBUG_MODE && cerr << "failed fork";
        request->set_file_path(INTERNAL_SERVER_ERROR);
        request->set_is_complete_post(true);
        request->set_is_cgi_complete(true);
        return;
    } else if (pid == 0) {
        webserv->close_kq();
        char **env = header_to_env();
        if (!env) {
            DEBUG_MODE && cerr << "new failed\n";
            exit(1);
        }
        if (DEBUG_MODE) print_env(env);
        
        int fd_write = open(file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_write < 0) {
            DEBUG_MODE && cerr << "can't open file in child\n";
            delete [] env;
            exit(2);
        }
        char *args[3];
        if (!request->get_cgi_path_post().empty()) {
            if (request->get_file_stream())
                request->get_file_stream()->close();
            string script_file_path = get_script_path();
            size_t pos = script_file_path.rfind(".");
            string extension = script_file_path.substr(pos + 1);
            
            DEBUG_MODE && cerr << "script_file_path: " << script_file_path << "\n";
            DEBUG_MODE && cerr << "input_file_path: " << request->get_cgi_path_post() << "\n";
            DEBUG_MODE && cerr << request->get_server().get_locations()[index_location].get_path_cgi(extension) << "\n";
            
            args[0] = const_cast<char *>(request->get_server().get_locations()[index_location].get_path_cgi(extension).c_str());
            args[1] = const_cast<char *>(get_script_path().c_str());
            args[2] = NULL;
            
            DEBUG_MODE && cerr << BOLD_RED << request->get_server().get_locations()[index_location].get_path_cgi(extension) << RESET << endl;
            DEBUG_MODE && cerr << BG_RED << "this is input file: " << request->get_cgi_path_post() << "\n" <<RESET << "\n";
            
            int fd_read = open(request->get_cgi_path_post().c_str(), O_RDWR);
            if (dup2(fd_read, 0) < 0) {
                DEBUG_MODE && cerr << "2) dup2 failed in child\n";
                close(fd_write);
                delete [] env;
                exit(3);
            }
        } else {
            size_t pos = request->get_file_path().rfind(".");
            string extension = request->get_file_path().substr(pos + 1);
            args[0] = const_cast<char *>(request->get_server().get_locations()[index_location].get_path_cgi(extension).c_str());
            args[1] = const_cast<char *>(request->get_file_path().c_str());
            args[2] = NULL;
        }
        if (dup2(fd_write, 1) < 0) {
            DEBUG_MODE && cerr << "1) dup2 failed in child\n";
            close(fd_write);
            delete [] env;
            exit(4);
        }
        execve(args[0], args, env);
        DEBUG_MODE && cerr << BOLD_RED << "execve failed\n" << RESET;
        close(fd_write);
        delete [] env;
        exit(5);
    
    } else {
        webserv->handle_timeout(pid, file_path, this);
    }
}
