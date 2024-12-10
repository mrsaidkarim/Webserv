/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skarim <skarim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/01 16:05:28 by skarim            #+#    #+#             */
/*   Updated: 2024/12/10 20:49:43 by skarim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

WebServ::WebServ()
{}

WebServ::WebServ(const vector<Server> &servers) : servers(servers)
{
}

WebServ::~WebServ()
{
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

void monitor_server_sockets(int kq, const map<int, vector<Server>> &servers)
{
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
            }
            else {
                // fd is the client socket file descriptor
                // servers related to socket is servers[client_server[fd]]
                //3ish a zechi
                

                char buffer[1024];
                int bytes_read = recv(fd, buffer, sizeof(buffer) - 1, 0);
                if (bytes_read <= 0) {
                    if (bytes_read == 0)
                        cout << "Client disconnected (fd: " << fd << ")\n";
                    else
                        perror("Error: Failed to read from client socket");

                    // Remove the client socket from kqueue and close it
                    close(fd);
                    continue;
                }
                string server_name = servers.at(client_server[fd])[0].get_server_names()[0];
                string response_body = "Hello from " + server_name + "!";
                string response = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(response_body.size()) + "\r\n\r\n" + response_body;

                send(fd, response.c_str(), response.size(), 0);
                
                close(fd);
            }
        }
    }
}

void WebServ::run_servers()
{
    map<int, vector<Server>> sockets_created;
    
    for(Server server : this->servers)
    {
        const vector<int> &ports = server.get_ports();
        const vector<string> &server_names = server.get_server_names();
        for(int port: ports)
        {
            sockets_created[port].push_back(server);
        }
    }
    for(map<int, vector<Server>>::iterator it = sockets_created.begin(); it != sockets_created.end(); ++it)
    {
        int server_socket;
        if (!configure_socket(server_socket, it->first))
            continue;
        socket_servers[server_socket] = it->second;
    }
    int kq = kqueue();
    if (kq == -1)
    {
        cerr << "Error: kqueue creation failed\n";
        return ;
    }
    if (!register_server_sockets(kq, socket_servers))
        return ;
    monitor_server_sockets(kq, socket_servers);
}

void WebServ::close_sockets()
{
    for(map<int, vector<Server>>::iterator it = socket_servers.begin(); it != socket_servers.end(); ++it)
    {
        close(it->first);
    }
}