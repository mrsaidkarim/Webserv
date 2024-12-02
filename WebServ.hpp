/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skarim <skarim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/01 15:23:39 by skarim            #+#    #+#             */
/*   Updated: 2024/12/01 22:24:26 by skarim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "const.hpp"
#include "Configuration/Server.hpp"
#include "HttpRequest.hpp"

class WebServ
{
    private:
        vector<Server> servers;
    public:
    WebServ();
    ~WebServ();
    bool run_one_server(const Server &server);
    void handle_client(int client_socket, const string &global_root);
    void run(); // Run All Servers
    // void listen();
    // void close_sockets();
};

#endif