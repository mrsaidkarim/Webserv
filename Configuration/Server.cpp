/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skarim <skarim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 12:53:50 by skarim            #+#    #+#             */
/*   Updated: 2024/11/30 14:06:08 by skarim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const vector<int> &ports, const vector<string> &server_names, const long long &client_max_body_size,
                const vector<Location> &locations, const string &global_root, const pair<string, string> &redirection,
                const vector<string> &indexes, bool autoindex, const vector<string> &error_pages) :
                ports(ports), server_names(server_names), client_max_body_size(client_max_body_size),
                locations(locations), global_root(global_root), redirection(redirection), indexes(indexes), autoindex(autoindex), 
                error_pages(error_pages)
{
    
}

const vector<int> &Server::get_ports(void) const
{
    return (ports);
}

const vector<string> &Server::get_server_names(void) const
{
    return (server_names);
}

const long long &Server::get_client_max_body_size(void) const
{
    return (client_max_body_size);
}

const vector<Location> &Server::get_locations(void) const
{
    return (locations);
}

const string &Server::get_global_root(void) const
{
    return (global_root);
}

const pair<string, string> &Server::get_redirection(void) const
{
    return (redirection);
}

const vector<string> &Server::get_indexes(void) const
{
    return (indexes);
}

bool Server::get_autoindex(void) const
{
    return (autoindex);
}

const vector<string> &Server::get_error_pages(void) const
{
    return (error_pages);
}