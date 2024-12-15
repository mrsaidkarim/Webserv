/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelabbas <zelabbas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/01 16:05:28 by skarim            #+#    #+#             */
/*   Updated: 2024/12/15 14:44:27 by zelabbas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include "Configuration/Server.hpp"
#include "Request/HttpRequest.hpp"
#include "Response/HttpResponse.hpp"
#include "const.hpp"
#include <sys/_types/_ssize_t.h>
#include <sys/event.h>

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

bool configure_socket(int &server_socket, int port)
{
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
        close(server_socket);
        return false;
    }

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (fcntl(server_socket, F_SETFL, O_NONBLOCK)) {
        cerr << "Error: failed to set server socket as non blocking\n";
        close(server_socket);
        return (false);
    }

    if (bind(server_socket, (sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        cerr << "Error: Failed to bind socket\n";
        close(server_socket);
        return (false);
    }
    if (listen(server_socket, SOMAXCONN) == -1)
    {
        cerr << "Error: Failed to listen on port " << port << "\n";
        close(server_socket);
        return (false);
    }
    return true;
}

bool register_server_sockets(int kq, const map<int, vector<Server>> &servers)
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
        perror("Error: kevent registration failed\n");
        return (false);
    }
    return (true);
}

// search for right server
Server host_server_name(const map<int, vector<Server>> &servers, int server_socket, HttpRequest *request) {
    map<int, vector<Server>>::const_iterator it = servers.find(server_socket);
    // I don't this this will happend but if it happend we should return some thing
    // if client connect to server so the server socket should be in the map
    if (it != servers.end()) {
        vector<Server> servers = it->second;
        return servers[0];
    }
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

    for (int i = 0; i < it->second.size(); i++) {
        // server_names is a vector of server names for current server in the vector of servers
        vector<string> server_names = it->second[i].get_server_names();
        for (int j = 0; j < server_names.size(); j++) {
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


void monitor_server_sockets(int kq, const map<int, vector<Server>> &servers)
{
    // set socket to non-blocking

    unordered_map<int, HttpResponse*> client_responses;
    const int MAX_EVENTS = 128; // may we change it after if wasn't perfect for memory usage
    struct kevent event_list[MAX_EVENTS];
    map<int, int> client_server;
    while (true) {
        int event_count = kevent(kq, nullptr, 0, event_list, MAX_EVENTS, nullptr);
        if (event_count == -1){
            perror("Error: kevent monitoring failed\n");
            continue;
        }
        for (int i = 0; i < event_count; ++i) {
            int fd = event_list[i].ident;

            if (servers.find(fd) != servers.end()) {
                // new client connection
                sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);

                int client_socket = accept(fd, (sockaddr *)&client_addr, &client_len);
                if (client_socket == -1){
                    perror("Error: Failed to accept connection\n");
                    continue;
                }
                cout << "Accepted connection on server socket (fd: " << fd << "), client fd: " << client_socket << "\n";
                struct kevent change;
                EV_SET(&change, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
                if (kevent(kq, &change, 1, nullptr, 0, nullptr) == -1) {
                    perror("Error: Failed to register client socket\n");
                    close(client_socket);
                    continue;
                }
                client_server[client_socket] = fd;
            } else if (event_list[i].filter == EVFILT_READ) {

                // read incoming request from client
                string serv_request_buffer = string(BUFFER_SIZE2, '\0');
                ssize_t bytes_read = recv(fd, &serv_request_buffer[0], BUFFER_SIZE2, 0);
                /*
                    MSG_DONTWAIT:
                    A flag indicating that the call should return immediately if no data is available, 
                    instead of blocking the process. If no data is available, recv returns -1 and sets
                    errno to EAGAIN or EWOULDBLOCK
                */
                if (bytes_read > 0) {
                    // process the request

					// ! HERE we should check is a new request or the the continue request that already existe! 
                    // get data from request buffer
                    HttpRequest *request = new HttpRequest(serv_request_buffer);

                    // search for right server
                    int server_socket = client_server[fd];
                    Server server = host_server_name(servers , server_socket, request);

                    // set client socket and server
                    request->set_client_socket(fd);
                    request->set_server(server);

                    // create response object
                    HttpResponse *response = new HttpResponse(request);

                    // store the response object in the map
                    client_responses[fd] = response;

                    // add client socket to kqueue for writing event
                    struct kevent change;
                    EV_SET(&change, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, nullptr);
                    kevent(kq, &change, 1, nullptr, 0, nullptr);

                } else {
                    // client disconnected
                    close(fd);
                }

            } else if (event_list[i].filter == EVFILT_WRITE) {
                // send chunked response to client
                HttpResponse *response = client_responses[fd];
                if (!response) {
                    close(fd);
                    continue;
                }
                response->serv();
                // if response is complete, remove all client data
                if (response->get_request()->get_is_complete()) {
                    delete response->get_request();
                    delete response;
                    close(fd);
                    client_responses.erase(fd);
                    struct kevent change;
                    EV_SET(&change, fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
                    kevent(kq, &change, 1, nullptr, 0, nullptr);
                }
            }
        }
    }
}

void WebServ::run_servers()
{
    // this map will store the server socket file descriptor as the key and the vector of servers that use that socket as the value
    // but i think you store some times key as port and some times as socket file descriptor
    map<int, vector<Server>> sockets_created;
    
    for(Server server : this->servers)
    {
        const vector<int> &ports = server.get_ports();
        const vector<string> &server_names = server.get_server_names();
        for(int port: ports)
        {
            sockets_created[port].push_back(server);
            cout << "to create socket for port: " << port << "\n"; 
        }
    }

    for(map<int, vector<Server>>::iterator it = sockets_created.begin(); it != sockets_created.end(); ++it)
    {
        int server_socket;
        // if (!configure_socket(server_socket, it->first)) that's mean socket creation failed
        if (!configure_socket(server_socket, it->first))
            continue; // should we exit the program.
        // i think you should erase the port
        socket_servers[server_socket] = it->second;
    }
    int kq = kqueue();
    if (kq == -1)
    {
        cerr << "Error: kqueue creation failed\n";
        return ; // should we exit the program.
    }
    if (!register_server_sockets(kq, socket_servers))
        return ; // should we exit the program.
    monitor_server_sockets(kq, socket_servers);
}

void WebServ::close_sockets()
{
    for(map<int, vector<Server>>::iterator it = socket_servers.begin(); it != socket_servers.end(); ++it)
    {
        close(it->first);
    }
}

const map<int, vector<Server>> &WebServ::get_socket_servers() const
{
    return socket_servers;
}