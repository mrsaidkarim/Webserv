/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skarim <skarim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 12:39:27 by skarim            #+#    #+#             */
/*   Updated: 2024/11/30 12:54:42 by skarim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "../const.hpp"
#include "Location.hpp"

class Server
{
    private:
        vector<int> ports;
        vector<string> server_names;
        long long client_max_body_size;
        vector<Location> locations;
        string global_root;
        pair<string, string> redirection;
        vector<string> indexes;
        bool autoindex;
        vector<string> error_pages;
    public:
        Server();
        Server(const vector<int> &ports, const vector<string> &server_names, const long long &client_max_body_size,
                const vector<Location> &locations, const string &global_root, const pair<string, string> &redirection,
                const vector<string> &indexes, bool autoindex, const vector<string> &error_pages);
        
        // GETTERS
        const vector<int> &get_ports(void) const;
        const vector<string> &get_server_names(void) const;
        const long long &get_client_max_body_size(void) const;
        const vector<Location> &get_locations(void) const;
        const string &get_global_root(void) const;
        const pair<string, string> &get_redirection(void) const;
        const vector<string> &get_indexes(void) const;
        bool get_autoindex(void) const;
        const vector<string> &get_error_pages(void) const;

        // SETTERS
        
};

#endif