/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelabbas <zelabbas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 12:53:50 by skarim            #+#    #+#             */
/*   Updated: 2024/12/19 16:23:49 by zelabbas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() {
	client_max_body_size = -1;
	autoindex = false;
	autoindex_set = false;
}

Server::Server(const vector<int> &ports, const vector<string> &server_names, const long long &client_max_body_size,
                const vector<Location> &locations, const string &global_root, const pair<string, string> &redirection,
                const vector<string> &indexes, bool autoindex, const map<string, string> &error_pages) :
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

const map<string, string> &Server::get_error_pages(void) const
{
    return (error_pages);
}

const string& Server::get_global_upload_store(void) const {
	return (global_upload_store);
}

// SETTERS

void Server::set_locations(Location& location) {
	locations.push_back(location);
}

void Server::set_ports(int port) {
	if (port != -1)
		this->ports.push_back(port);
}

void Server::set_error_pages(const string& key,const string& value) {
	this->error_pages[key] = value;
}

bool Server::set_server_names(const vector<string>& vec) {
	// this is check duplicate in config file!
	if (!this->server_names.empty())
		return (false);
	for (int i = 1; i < vec.size(); i++)
	{
		this->server_names.push_back(vec[i]);
	}
	return (true);
}

bool Server::set_global_root(const string& root) {
	// to check if the path exist !
	// this is check duplicate in config file!
	if (!this->global_root.empty())
		return (false);
	this->global_root = root;
	return (true);
}

bool Server::set_global_upload_store(const string& upload_stor) {
	// to check if the path exist !
	// this is check duplicate in config file!
	if (!this->global_upload_store.empty())
		return (false);
	this->global_upload_store = upload_stor;
	return (true);
}

bool Server::set_indexes(const vector<string>& vec) {
	// this is check duplicate in config file!
	if (!this->indexes.empty())
		return (false);
	for (int i = 1; i < vec.size(); i++)
	{
		this->indexes.push_back(vec[i]);
	}
	return (true);
}

bool Server::set_autoindex(bool auto_index) {
	// this is check duplicate in config file!
	if (autoindex_set)
		return (false);
	this->autoindex = auto_index;
	autoindex_set = true;
	return (true);
}

bool Server::set_redirection(const pair<string, string>& _redirection) {
	// this is check duplicate in config file!
	if (!redirection.first.empty() && !redirection.second.empty())
		return (false);
	this->redirection = _redirection;
	return (true);
}

bool Server::set_client_max_body_size(const string& str_value) {

	// this is check duplicate in config file!
	if (client_max_body_size != -1)
		return (false);
	stringStream ss(str_value);
	ss >> client_max_body_size;
	if (ss.fail() || !ss.eof())
		return (false);
	if (client_max_body_size < 0)
		return (false);
	return (true);
}


void Server::print_server_info(void) const {
    cout << BOLD_YELLOW << "********************* server info *********************" << RESET << endl;
    cout << BG_BLACK;

    cout << BOLD_BLUE << left << setw(20) << "ports " << ": " << BOLD_WHITE << "[";
    for (int i = 0; i < ports.size(); i++) {
        if (i > 0)
            cout << ", ";
        cout << ports[i];
    }
    cout << "]" << endl;

    cout << BOLD_BLUE << left << setw(20) << "server names " << ": " << BOLD_WHITE << "[";
    for (int i = 0; i < server_names.size(); i++) {
        if (i > 0)
            cout << ", ";
        cout << server_names[i];
    }
    cout << "]" << endl;

    cout << BOLD_BLUE << left << setw(20) << "indexes " << ": " << BOLD_WHITE << "[";
    for (int i = 0; i < indexes.size(); i++) {
        if (i > 0)
            cout << ", ";
        cout << indexes[i];
    }
    cout << "]" << endl;

    cout << BOLD_BLUE << left << setw(20) << "error_pages " << ": " << BOLD_WHITE << "[";
	int i = 0;
    for (auto it = error_pages.begin() ; it != error_pages.end(); it++) {
        if (i > 0)
            cout << ", ";
        cout << it->first << ": " << it->second;
		i++;
    }
    cout << "]" << endl;

    cout << BOLD_BLUE << left << setw(20) << "client_max_body_size" << ": " << BOLD_WHITE << client_max_body_size << " Byte\n";
    cout << BOLD_BLUE << left << "" << "global_upload_store: " << BOLD_WHITE << global_upload_store << "\n";

    cout << BOLD_BLUE << left << setw(20) << "global_root " << " :" << BOLD_WHITE << global_root << "\n";

    cout << BOLD_BLUE << left << setw(20) <<  "redirection " << ": " << BOLD_WHITE;
    cout << "[" << redirection.first << "]" << " [" << redirection.second << "]\n";

    cout << BOLD_BLUE << left << setw(20) << "auto_index " << ": " << BOLD_WHITE;
    if (autoindex) cout << "on" << endl;
    else cout << "off" << endl;

    for (int i = 0; i < locations.size(); i++) {
        cout << BOLD_BLUE << left << setw(20) << "location "  << ": " << BOLD_WHITE << i + 1 << "\n";
        locations[i].print_lacation_info();
    }
}