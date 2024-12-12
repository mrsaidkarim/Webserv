/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skarim <skarim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/01 15:23:39 by skarim            #+#    #+#             */
/*   Updated: 2024/12/10 18:17:42 by skarim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "const.hpp"
#include "Configuration/Server.hpp"
#include "Request/HttpRequest.hpp"

class WebServ
{
    private:
        vector<Server> servers;
        map<int, vector<Server>> socket_servers;
    public:
        WebServ();
        WebServ(const vector<Server> &servers);
        ~WebServ();
        void run_servers(); // Run All Servers
        void close_sockets();

        const map<int, vector<Server>> &get_socket_servers() const;
};

#endif