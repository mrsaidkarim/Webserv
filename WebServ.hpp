/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skarim <skarim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/01 15:23:39 by skarim            #+#    #+#             */
/*   Updated: 2025/01/16 19:19:50 by skarim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "const.hpp"
#include "Configuration/Server.hpp"
#include "Request/HttpRequest.hpp"
#include "Response/HttpResponse.hpp"
#include <sys/_types/_pid_t.h>

class HttpResponse;

class WebServ
{
    private:
        vector<Server> servers;
        map<int, vector<Server>> socket_servers;
        unordered_map<pid_t, pair<const HttpResponse*, int> > pid_childs;
        unordered_map<pid_t, string> file_paths;
        int kq;

    public:
        WebServ();
        WebServ(const vector<Server> &servers);
        ~WebServ();
        void run_servers(); // Run All Servers
        void close_sockets();
        void close_kq();
        void handle_timeout(pid_t pid, const string& file_path, const HttpResponse *response);

        const unordered_map<pid_t, pair<const HttpResponse*, int> >& get_pid_childs() const;
        const unordered_map<pid_t, string>& get_file_paths() const;
        const map<int, vector<Server>> &get_socket_servers() const;
        void	set_servers(Server& server);


        // to remove 
        void print_all_servers();
};

#endif