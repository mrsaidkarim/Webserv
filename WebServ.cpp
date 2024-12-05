/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skarim <skarim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/01 16:05:28 by skarim            #+#    #+#             */
/*   Updated: 2024/12/04 15:35:32 by skarim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

WebServ::WebServ()
{}
WebServ::~WebServ()
{}

// this is gonna be the final version of run_one_server (each server from the vector of servers)
// void WebServ::run_one_server(const Server &server)
// {
//     const vector<int> &ports = server.get_ports();
//     const string &global_root = server.get_global_root();
    
//     for(size_t i = 0; i < ports.size(); i++)
//     {
        
//     }
// }

bool    configure_socket(sockaddr_in &server_addr, const string &server_name, int port)
{
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    addrinfo helper, *res;
    memset(&helper, 0, sizeof(helper));
    helper.ai_family = AF_INET;
    helper.ai_socktype = SOCK_STREAM;
    
    if (getaddrinfo(server_name.c_str(), nullptr, &helper, &res))
    {
        cerr << "Error: Unable to resolve server_name: " << server_name << "\n";
        return (false);
    }
    sockaddr_in *addr = (sockaddr_in *)res->ai_addr;
    server_addr.sin_addr = addr->sin_addr;
    freeaddrinfo(res);
    
    // display the ip address just to check lkhedma wesh n9ia
    // char ip_buffer[INET_ADDRSTRLEN];
    // inet_ntop(AF_INET, &(server_addr.sin_addr), ip_buffer, INET_ADDRSTRLEN);
    // cout << "Resolved IP for server_name '" << server_name << "': " << ip_buffer << "\n";

    return (true);
}

void    WebServ::handle_client(int client_socket, const string &global_root)
{
    char    buffer[BUFFER_SIZE];
    int     bytes_received;
    string  full_request;
    
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received < 0)
        {
            cerr << "Error: Failed to read client request\n";
            return ;
        }
        else if (bytes_received = 0)
            break;
        full_request.append(buffer, bytes_received);
    }
    // Display the full request
    cout << "Received request: \n" << full_request << "\n";
    HttpRequest http_request(full_request);
}

// This version in case of one server listen on one port
bool WebServ::run_one_server(const Server &server)
{
    int port = server.get_ports()[0];
    const string &global_root = server.get_global_root();
    const string &server_name = server.get_server_names()[0];
    
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        cerr << "Error: Socket creation failed\n";
        return (false);
    }
    sockaddr_in server_addr;
    if (!configure_socket(server_addr, server_name, port))
    {
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
    cout << "Server is listening on port " << port << "....\n";
    while (true)
    {
        sockaddr client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_socket = accept(server_socket, (sockaddr *)&client_addr, &client_len);
        if (client_socket == -1)
        {
            cerr << "Error: Failed to accept connection\n";
            continue;
        }
        handle_client(client_socket, global_root);
        close(client_socket);
    }
    close(server_socket);
    return (true);
}

// Run All Servers
void WebServ::run()
{
    
}

// void WebServ::listen();
// void WebServ::close_sockets();