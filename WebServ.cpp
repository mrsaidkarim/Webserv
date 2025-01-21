/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skarim <skarim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/01 16:05:28 by skarim            #+#    #+#             */
/*   Updated: 2025/01/20 22:43:47 by skarim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <cstdlib>
#include <exception>
#include <sys/wait.h>
// int kq;
#include "WebServ.hpp"
#include "Configuration/Server.hpp"
#include "Request/HttpRequest.hpp"
#include "Response/HttpResponse.hpp"
#include "const.hpp"
#include <fstream>
#include <sys/_types/_ssize_t.h>
#include <sys/event.h>
#include <unistd.h>

WebServ::WebServ()
{
    cout << "default WebServ constructor called\n";
}

WebServ::WebServ(const vector<Server> &servers) : servers(servers)
{
    cout << "parameterized WebServ constructor called\n";
}

WebServ::~WebServ()
{
    cout << "WebServ destructor called\n";
    this->close_sockets();
}

// bool configure_socket(int &server_socket, int port)
// {
//     server_socket = socket(AF_INET, SOCK_STREAM, 0);
//     if (server_socket == -1)
//     {
//         cerr << "Error: Socket creation failed\n";
//         return (false);
//     }
//     int optval = 1;
//     if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
//     {
//         cerr << "Error: Failed to set socket options\n";
//         close(server_socket);
//         return false;
//     }

//     sockaddr_in server_addr;
//     memset(&server_addr, 0, sizeof(server_addr));
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_port = htons(port);
//     server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

//     if (fcntl(server_socket, F_SETFL, O_NONBLOCK)) {
//         cerr << "Error: failed to set server socket as non blocking\n";
//         close(server_socket);
//         return (false);
//     }

//     if (bind(server_socket, (sockaddr *)&server_addr, sizeof(server_addr)) == -1)
//     {
//         cerr << "Error: Failed to bind socket\n";
//         close(server_socket);
//         return (false);
//     }
//     if (listen(server_socket, SOMAXCONN) == -1)
//     {
//         cerr << "Error: Failed to listen on port " << port << "\n";
//         close(server_socket);
//         return (false);
//     }
//     return true;
// }
bool configure_socket(int &server_socket, pair<int, string> port_host)
{
    int port = port_host.first;
    string host = port_host.second;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        cerr << "Error: Socket creation failed\n";
        return (false);
    }
    int optval = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
    {
        cerr << "Error: Failed to set socket options\n";
        return false;
    }

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (host.empty())
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    else {
        // server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addrinfo helper, *res = NULL;
        memset(&helper, 0, sizeof(helper));
        helper.ai_family = AF_INET;
        helper.ai_socktype = SOCK_STREAM;

        char port_str[6]; //port form int to string
        sprintf(port_str, "%d", port);

        if (getaddrinfo(host.c_str(), port_str, &helper, &res) != 0)
        {
            cerr << "Error: Failed to resolve host: " << host << "\n";
            return (false);
        }
        sockaddr_in *resolved_addr = reinterpret_cast<sockaddr_in *>(res->ai_addr);
        server_addr.sin_addr = resolved_addr->sin_addr;
        freeaddrinfo(res);
    }

    if (fcntl(server_socket, F_SETFL, O_NONBLOCK) == -1) {
        cerr << "Error: failed to set server socket as non blocking\n";
        return (false);
    }

    if (bind(server_socket, (sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        cerr << "Error: Failed to bind socket\n";
        return (false);
    }
    if (listen(server_socket, SOMAXCONN) == -1)
    {
        cerr << "Error: Failed to listen on port " << port << "\n";
        return (false);
    }
    return (true);
}

void register_server_sockets(int kq, const map<int, vector<Server>> &servers)
{
    vector<struct kevent> change_list;

    for (map<int, vector<Server>>::const_iterator it = servers.begin(); it != servers.end(); it++)
    {
        int server_socket = it->first;
        struct kevent change;
        EV_SET(&change, server_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
        change_list.push_back(change);
    }

    if (kevent(kq, change_list.data(), change_list.size(), nullptr, 0, nullptr) == -1)
    {
        cerr << "Error: kevent registration failed\n";
        for(map<int, vector<Server>>::const_iterator it2 = servers.begin(); it2 != servers.end(); ++it2) {
            close(it2->first);
        }
        close(kq);
        exit(EXIT_FAILURE);
    }
}

// search for right server
Server host_server_name(const map<int, vector<Server>> &servers, int server_socket, HttpRequest *request) {
    map<int, vector<Server>>::const_iterator it = servers.find(server_socket);
    // I don't this this will happend but if it happend we should return some thing
    // if client connect to server so the server socket should be in the map
    Server server;
    // if host header is not found in the request
    // we return the first server in the vector
    if (request->get_header().find("host") == request->get_header().end()) { // update here from zak from key = Host to host
        server = it->second[0];
        return server;
    }

    // the host header is found in the request
    string host = request->get_header().find("host")->second; // update here from zak from key = Host to host
    bool found = false;

    for (size_t i = 0; i < it->second.size(); i++) {
        // server_names is a vector of server names for current server in the vector of servers
        vector<string> server_names = it->second[i].get_server_names();
        for (size_t j = 0; j < server_names.size(); j++) {
            if (server_names[j] == host) {
                server = it->second[i];
                found = true;
                break;
            }
        }
        if (found) {
            break;
        }
    }

    // if not found return the first server in the vector
    if (!found) {
        server = it->second[0];
    }
    return (server);
}

// static string addPrefixBeforeCRLF(const string &input) {
//     const string word = "\r\n";
//     const string prefix = "{$$$$}";
//     string result = input;
//     size_t pos = 0;

// }
void process_request(unordered_map<int, HttpResponse*> &client_responses, map<int, int> &client_server,
                        const map<int, vector<Server>> &servers, int &kq, int &fd, WebServ* webserv)
{
    string serv_request_buffer = string(BUFFER_SIZE2, 0);
    ssize_t bytes_read = recv(fd, &serv_request_buffer[0], BUFFER_SIZE2, 0);
    // cout << BOLD_YELLOW << serv_request_buffer << RESET << "\n";
    if (bytes_read > 0) 
    { 
        // process the request
        serv_request_buffer.resize(bytes_read);
        if (client_responses.find(fd) == client_responses.end()){ // new request
            cerr << BOLD_GREEN << "ADD SOCKET  >> :" << fd << "\n" << RESET;
            HttpRequest *request;
            try {
                request = new HttpRequest(serv_request_buffer);
            } catch (std::exception& e) {
                cerr << BOLD_RED << "new failed: " << e.what() << "\n" << RESET;
                return ;
            }
            // HttpRequest *request = new HttpRequest(serv_request_buffer);
            int server_socket = client_server[fd];
            request->set_client_socket(fd);
            HttpResponse *response;
            try {
                response = new HttpResponse(request, webserv);
            } catch (std::exception& e) {
                delete  request;
                cerr << BOLD_RED << "new failed: " << e.what() << "\n" << RESET;
                return ;
            }
            request->http_request_init();
            Server server = host_server_name(servers , server_socket, request);
            request->set_server(server);
            client_responses[fd] = response;
            map<string, string>	header = request->get_header();
            if (request->get_method() == "POST" ) {
                response->serv();
                cerr << BG_BLUE << "in it" << "\n" << RESET;
            } 
            if (response->get_request()->get_is_complete_post() || request->get_method() == "GET" || request->get_method() == "DELETE")
            {
                cerr << BG_GREEN << "in it 2" << "\n" << RESET;
                struct kevent change;
                if (response->get_request()->get_method() == "POST") {
                    response->get_request()->set_method("GET");
                    response->get_request()->set_is_chunked(true);
                    if (response->get_request()->get_is_cgi()) {
                        response->serv();
                    } else {
                        EV_SET(&change, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, nullptr);
                        if (kevent(kq, &change, 1, nullptr, 0, nullptr) == -1) {
                            cerr << "Error: Failed to re-register client socket for writing";
                            delete response->get_request();
                            delete response;
                            close(fd);
                            client_responses.erase(fd);
                            struct kevent change;
                            EV_SET(&change, fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
                            cerr << BOLD_RED << "DELETE SOCKET  >> :" << fd << "\n" << RESET;
                            kevent(kq, &change, 1, nullptr, 0, nullptr);
                        }
                    }
                } else {
                    EV_SET(&change, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, nullptr);
                    if (kevent(kq, &change, 1, nullptr, 0, nullptr) == -1) {
                        cerr << "Error: Failed to re-register client socket for writing";
                        delete response->get_request();
                        delete response;
                        close(fd);
                        client_responses.erase(fd);
                        struct kevent change;
                        EV_SET(&change, fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
                        cerr << BOLD_RED << "DELETE SOCKET  >> :" << fd << "\n" << RESET;
                        kevent(kq, &change, 1, nullptr, 0, nullptr);
                    }
                }
                return ;
            }
        }
        else { // case of post_method continuation of reading the request body
            cerr << BG_BLUE << "in ittt\n" << RESET;
            struct kevent change;
            HttpResponse *response = client_responses[fd];
            if (!response || !response->get_request())
            {
                close(fd);
                return ;
            }
            HttpRequest *request = response->get_request();
            request->add_to_body(serv_request_buffer, bytes_read);
            response->serv();
            if (response->get_request()->get_is_complete_post())
            {
                response->get_request()->set_method("GET");
                response->get_request()->set_is_chunked(true);

                cerr << BG_GREEN << "from post to get" << RESET<< "\n";
                if (response->get_request()->get_is_cgi()) {
                    response->serv();
                } else {
                    EV_SET(&change, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, nullptr);
                    if (kevent(kq, &change, 1, nullptr, 0, nullptr) == -1) {
                        cerr << "Error: Failed to re-register client socket for writing\n" ;
                        delete response->get_request();
                        delete response;
                        close(fd);
                        client_responses.erase(fd);
                        struct kevent change;
                        EV_SET(&change, fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
                        cerr << BOLD_RED << "DELETE SOCKET  >> :" << fd << "\n" << RESET;
                        kevent(kq, &change, 1, nullptr, 0, nullptr);
                    }
                }  
            }
        }
    } else if (bytes_read == 0) {
        cerr << "failed to upload\n\n\n\n";
        if (client_responses.find(fd) != client_responses.end())
            cerr << "init\n\n";
        HttpResponse *response = client_responses[fd];
        if (!response || !response->get_request())
        {
            close(fd);
            return ;
        }
        if (response->get_request()->get_method() == "POST") {
            if (response->get_request()->get_file_stream())
                response->get_request()->get_file_stream()->close();
            delete response->get_request();
            delete response;
            struct kevent change;
            EV_SET(&change, fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
            kevent(kq, &change, 1, nullptr, 0, nullptr);
            client_responses.erase(fd);
            client_server.erase(fd);
            close(fd);
        }
    } else if (bytes_read == -1) {
        cerr << BOLD_RED << "bytes_read == -1\n" << RESET;
        return;
    }
}

static string generate_session_id(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    // Extract seconds and microseconds
    time_t rawTime = tv.tv_sec;
    int microseconds = tv.tv_usec;
    
    // Convert to local time
    struct tm *timeInfo = localtime(&rawTime);

    // Create the formatted string
    ostringstream oss;
    oss << (timeInfo->tm_year + 1900) << "_"       // Full Year (e.g., 2025)
        << (timeInfo->tm_mon + 1) << "_"           // Month
        << timeInfo->tm_mday << "_"               // Day
        << timeInfo->tm_hour << "_"               // Hours
        << timeInfo->tm_min << "_"                // Minutes
        << timeInfo->tm_sec << "_"                // Seconds
        << microseconds;               // Microseconds

    return oss.str();
}

void WebServ::handle_timeout(pid_t pid, const string& file_path, const HttpResponse *response) {
    struct kevent change;

    // Monitor the child process exit event
    EV_SET(&change, pid, EVFILT_PROC, EV_ADD | EV_ENABLE, NOTE_EXIT, 0, nullptr);
    if (kevent(kq, &change, 1, nullptr, 0, nullptr) == -1) {
        cerr << "Failed to monitor child process\n";
        kill(pid, SIGKILL);
        response->get_request()->set_status_code("500");
        response->send_response();
        return;
    }

    // Add timeout monitoring (this is a timer that triggers if the child takes too long)
    struct kevent timeout_event;
    EV_SET(&timeout_event, pid, EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, CGI_TIMEOUT, nullptr);  // Timeout in milliseconds
    if (kevent(kq, &timeout_event, 1, nullptr, 0, nullptr) == -1) {
        cerr << BOLD_RED << "Failed to add timeout event\n" << RESET;
        kill(pid, SIGKILL);
        response->get_request()->set_status_code("500");
        response->send_response();
        return;
    }

    cout << "---> " << response->get_request()->get_client_socket() << " " << pid << endl;
    pid_childs[pid] = make_pair(response, response->get_request()->get_client_socket());
    file_paths[pid] = file_path;
}

bool copy_file(const string& sourcePath, const string& destinationPath) {
    // Open the source file for reading in binary mode
    ifstream sourceFile(sourcePath.c_str(), ios::binary);
    if (!sourceFile) {
        cerr << "Error: Could not open source file: " << sourcePath << endl;
        return false;
    }

    // Open the destination file for writing in binary mode
    ofstream destinationFile(destinationPath.c_str(), ios::binary);
    if (!destinationFile) {
        cerr << "Error: Could not open destination file: " << destinationPath << endl;
        return false;
    }

    // Copy contents from source to destination
    destinationFile << sourceFile.rdbuf();

    // Close the files
    sourceFile.close();
    destinationFile.close();

    return true;
}

void monitor_server_sockets(int kq, const map<int, vector<Server>> &servers, WebServ* webserv)
{
    unordered_map<int, HttpResponse*> client_responses;
    const int MAX_EVENTS = 128; // may we change itif wa after sn't perfect for memory usage
    struct kevent event_list[MAX_EVENTS];
    map<int, int> client_server;
    while (true) {
        int event_count = kevent(kq, nullptr, 0, event_list, MAX_EVENTS, nullptr);
        if (event_count == -1){
            cerr << "Error: kevent monitoring failed\n";
            continue;
        }
        for (int i = 0; i < event_count; ++i) {
            int fd = event_list[i].ident;

            if (servers.find(fd) != servers.end()) { // new client connection
                sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_socket = accept(fd, (sockaddr *)&client_addr, &client_len);
                if (client_socket == -1){
                    cerr << "Error: Failed to accept connection\n";
                    continue;
                }
                cout << "Accepted connection on server socket (fd: " << fd << "), client fd: " << client_socket << "\n";
                if (fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1) {
                    cerr << "failed to set client socket as non blocking mode\n";
                    close(client_socket);
                    continue;
                }
                struct kevent change;
                EV_SET(&change, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
                if (kevent(kq, &change, 1, nullptr, 0, nullptr) == -1) {
                    cerr << "Error: Failed to register client socket\n";
                    close(client_socket);
                    continue;
                }
                client_server[client_socket] = fd;
            } else if (event_list[i].filter == EVFILT_READ) { // read incoming request from client
                process_request(client_responses, client_server, servers, kq, fd, webserv);
            } else if (event_list[i].filter == EVFILT_WRITE) {
                HttpResponse *response = client_responses[fd];
                if (!response || !response->get_request()) {
                    close(fd);
                    continue;
                }
                response->serv();
                if (response->get_request()->get_is_complete()) {
                    delete response->get_request();
                    delete response;
                    close(fd);
                    client_responses.erase(fd);
                    struct kevent change;
                    EV_SET(&change, fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
                    cerr << BOLD_RED << "DELETE SOCKET  >> :" << fd << "\n" << RESET;
                    kevent(kq, &change, 1, nullptr, 0, nullptr);
                }
            } else if (event_list[i].filter == EVFILT_PROC) {
                cout << BOLD_RED <<  "CHILD exit\n" << RESET;
                // Child process exit event
                pid_t child_pid = event_list[i].ident; // The pid of the child that triggered the event
                unordered_map<pid_t, pair<const HttpResponse*, int> >::const_iterator it = webserv->get_pid_childs().find(child_pid);
                unordered_map<pid_t, string>::const_iterator it2 = webserv->get_file_paths().find(child_pid);
                const HttpResponse *response = it->second.first;
                int status  = -1;
                waitpid(child_pid, &status, WNOHANG);

                if (WIFEXITED(status)) {
                    int exit_status = WEXITSTATUS(status);
                    cerr << "Child process " << child_pid << " exited with status: " << exit_status << "\n";

                    if (exit_status == 0 || exit_status == 10) {
                        cout << BOLD_BLUE << "(exit_status == 0 || exit_status == 10)\n" << "cookie = " << response->get_request()->get_cookie() << RESET;
                        if (exit_status == 0 && response->get_request()->get_cookie() == 1) {
                            ///  add condition when we should add session_id
                            cout << BOLD_BLUE << "response->get_request()->get_cookie() == 1\n" << RESET;
                            response->get_request()->set_session_id(generate_session_id());
                            copy_file(it2->second, SESSION_MANAGEMENT + response->get_request()->get_session_id());
                        }

                        cerr << BG_GREEN << "this file return: >>>>>>  " << it2->second << "\n" << RESET;
                        response->get_request()->set_file_path(it2->second); // Successful CGI execution
                    } else {
                        cerr << "CGI process exited with error status: " << exit_status << "\n";
                        response->get_request()->set_status_code("500");
                    }
                } else if (WIFSIGNALED(status)) {
                    int signal_num = WTERMSIG(status);
                    cerr << "Child process " << child_pid << " was terminated by signal: " << signal_num << "\n";
                    response->get_request()->set_status_code("500");
                } else {
                    cerr << "Unexpected child process exit status\n";
                    response->get_request()->set_status_code("500");
                }

                // Mark request as complete and send response
                struct kevent change;
                struct kevent timeout_event;

                // update client socket to write mode
                // check for errors
                EV_SET(&change, it->second.second, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, nullptr);
                if (kevent(kq, &change, 1, nullptr, 0, nullptr) == -1) {
                    cerr << "Error: Failed to re-register client socket for writing!!!!!!!!\n";
                    delete response->get_request();
                    delete response;
                    close(it->second.second);
                    client_responses.erase(it->second.second);
                    struct kevent change;
                    EV_SET(&change, it->second.second, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
                    cerr << BOLD_RED << "DELETE SOCKET  >> :" << fd << "\n" << RESET;
                    kevent(kq, &change, 1, nullptr, 0, nullptr);
                    continue;
                }

                response->get_request()->set_is_cgi(false); // ok
                response->get_request()->set_is_cgi_complete(true);
                response->get_request()->set_method("GET");
                response->get_request()->set_is_chunked(true);
                response->send_response();

                // Remove the timeout event
                EV_SET(&timeout_event, child_pid, EVFILT_TIMER, EV_DELETE, 0, 0, nullptr);
                kevent(kq, &timeout_event, 1, nullptr, 0, nullptr);
            } else if (event_list[i].filter == EVFILT_TIMER) {
                cout << BOLD_RED << "CHILD exit timeout \n" << RESET;
                pid_t child_pid = event_list[i].ident; // The pid of the child that triggered the event
                unordered_map<pid_t, pair<const HttpResponse*, int> >::const_iterator it = webserv->get_pid_childs().find(child_pid);
                const HttpResponse *response = it->second.first;
                // if (!response) {
                //     close(fd);
                //     continue;
                // }
                // Timeout event triggered (if CGI process takes too long)
                struct kevent change;
                struct kevent timeout_event;

                response->get_request()->set_status_code("408");
                response->get_request()->set_file_path(REQUEST_TIMEOUT);
                response->get_request()->set_is_cgi_complete(true);
                response->get_request()->set_is_cgi(false); // ok
                response->get_request()->set_method("GET");
                response->get_request()->set_is_chunked(true);
                // response->send_response();
                // update client socket to write mode
                // check for errors
                EV_SET(&change, it->second.second, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, nullptr);
                if (kevent(kq, &change, 1, nullptr, 0, nullptr) == -1) {
                    cerr << "Error: Failed to re-register client socket for writing\n";
                    delete response->get_request();
                    delete response;
                    close(it->second.second);
                    client_responses.erase(it->second.second);
                    struct kevent change;
                    EV_SET(&change, it->second.second, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
                    kevent(kq, &change, 1, nullptr, 0, nullptr);
                    continue;
                }

                EV_SET(&change, event_list[i].ident, EVFILT_PROC, EV_DELETE, 0, 0, nullptr);
                kevent(kq, &change, 1, nullptr, 0, nullptr);
                kill(event_list[i].ident, SIGKILL);  // Kill the child process if it's still running
                // Remove the timeout event
                EV_SET(&timeout_event, event_list[i].ident, EVFILT_TIMER, EV_DELETE, 0, 0, nullptr);
                kevent(kq, &timeout_event, 1, nullptr, 0, nullptr);
            }
        }
    }
}

void WebServ::run_servers()
{
    // this map will store the server socket file descriptor as the key and the vector of servers that use that socket as the value
    // but i think you store some times key as port and some times as socket file descriptor
    map<pair<int, string>, vector<Server> > sockets_created;
    
    for(Server server : this->servers)
    {
        const vector<pair<int, string> > &ports_hosts = server.get_ports();
        // const vector<string> &server_names = server.get_server_names();
        for(pair<int, string> port_host: ports_hosts)
        {
            sockets_created[port_host].push_back(server);
            cout << "to create socket for port: " << port_host.first << " host: " << port_host.second << "\n"; 
        }
    }

    for(map<pair<int, string>, vector<Server>>::iterator it = sockets_created.begin(); it != sockets_created.end(); ++it)
    {
        int server_socket;
        if (!configure_socket(server_socket, it->first)) {
            //that's mean socket creation failed
            close(server_socket);
            // close all sockets already created
            for(map<int, vector<Server>>::iterator it2 = socket_servers.begin(); it2 != socket_servers.end(); ++it2) {
                close(it2->first);
            }
            cerr << BOLD_RED << "failed to init servers\n" << RESET;
            exit(EXIT_FAILURE);
        }
        // i think you should erase the port
        socket_servers[server_socket] = it->second;
    }
    this->kq = kqueue();
    // kq = kqueue();
    if (kq == -1)
    {
        cerr << "Error: kqueue creation failed\n";
        // close all sockets already created
        for(map<int, vector<Server>>::iterator it2 = socket_servers.begin(); it2 != socket_servers.end(); ++it2) {
            close(it2->first);
        }
        exit(EXIT_FAILURE);
        // return ; // should we exit the program.
    }
    // if (!register_server_sockets(kq, socket_servers))
    //     return ; // should we exit the program.
    register_server_sockets(kq, socket_servers);
    monitor_server_sockets(kq, socket_servers, this);
}
// void WebServ::run_servers()
// {
//     // this map will store the server socket file descriptor as the key and the vector of servers that use that socket as the value
//     // but i think you store some times key as port and some times as socket file descriptor
//     map<int, vector<Server>> sockets_created;
    
//     for(Server server : this->servers)
//     {
//         const vector<int> &ports = server.get_ports();
//         // const vector<string> &server_names = server.get_server_names();
//         for(int port: ports)
//         {
//             sockets_created[port].push_back(server);
//             cout << "to create socket for port: " << port << "\n"; 
//         }
//     }

//     for(map<int, vector<Server>>::iterator it = sockets_created.begin(); it != sockets_created.end(); ++it)
//     {
//         int server_socket;
//         // if (!configure_socket(server_socket, it->first)) that's mean socket creation failed
//         if (!configure_socket(server_socket, it->first))
//             continue; // should we exit the program.
//         // i think you should erase the port
//         socket_servers[server_socket] = it->second;
//     }
//     this->kq = kqueue();
//     // kq = kqueue();
//     if (kq == -1)
//     {
//         cerr << "Error: kqueue creation failed\n";
//         return ; // should we exit the program.
//     }
//     if (!register_server_sockets(kq, socket_servers))
//         return ; // should we exit the program.
//     monitor_server_sockets(kq, socket_servers, this);
// }

void WebServ::close_sockets()
{
    for(map<int, vector<Server>>::iterator it = socket_servers.begin(); it != socket_servers.end(); ++it)
    {
        close(it->first);
    }
}

void WebServ::close_kq()
{
    close(this->kq);
}


const map<int, vector<Server>> &WebServ::get_socket_servers() const
{
    return socket_servers;
}

const unordered_map<pid_t, pair<const HttpResponse*, int> >& WebServ::get_pid_childs() const {
    return (pid_childs);
}

const unordered_map<pid_t, string>& WebServ::get_file_paths() const {
    return (file_paths);
}

void WebServ::set_servers(Server& server) {
	servers.push_back(server);
}

bool WebServ::is_servers_empty() const {
    return (servers.empty());
}

void WebServ::print_all_servers() {
  for(size_t i = 0; i < servers.size(); i++) {
    servers[i].print_server_info();

    cout << BOLD_RED << "end server!" << RESET;
  }
}