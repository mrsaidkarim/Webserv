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
#include <cstddef>

Server::Server() {
	cout << BOLD_CYAN << "new server"<< RESET;
    client_max_body_size = -1;
	autoindex_set = false;
	autoindex = false;
}

Server::Server(const Server& _server) {
	cout << BOLD_CYAN << "copy server"<< RESET;
	if (this != &_server)
		*this = _server;
}

Server&	Server::operator=(const Server& _server) {

	cout << BOLD_CYAN << "ASSIGNEMT server"<< RESET;
	if (this != &_server) {
		this->global_root = _server.global_root;
		// this->host_name = _server.host_name;
		this->ports = _server.ports;
		this->server_names = _server.server_names;
		this->client_max_body_size = _server.client_max_body_size;
		this->locations = _server.locations;
		this->redirection = _server.redirection;
		this->indexes = _server.indexes;
		this->autoindex = _server.autoindex;
		this->error_pages = _server.error_pages;
		this->autoindex_set = _server.autoindex_set;
	}
	return (*this);
}

// Server::Server(const vector<int> &ports, const vector<string> &server_names, const long long &client_max_body_size,
//                 const vector<Location> &locations, const string &global_root, const pair<string, string> &redirection,
//                 const vector<string> &indexes, bool autoindex, const map<string, string> &error_pages) :
//                 ports(ports), server_names(server_names), client_max_body_size(client_max_body_size),
//                 locations(locations), global_root(global_root), redirection(redirection), indexes(indexes), autoindex(autoindex), 
//                 error_pages(error_pages)
// {
// }

const vector<pair<int, string> >& Server::get_ports(void) const
{
    return (ports);
}

const vector<string> &Server::get_server_names(void) const
{
    return (server_names);
}

const long long &Server::get_client_max_body_size(void) const
{
	cout << "here from get method!!! >>>>> " << client_max_body_size;
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

// const string& Server::get_host_name(void) const {
// 	return (host_name);
// }

// const string& Server::get_global_upload_store(void) const {
// 	return (global_upload_store);
// }

// SETTERS

void Server::set_locations(Location& location) {
	locations.push_back(location);
}

void Server::set_ports(int port,const string& _host) {
	// if (port != -1)
	// 	this->ports.push_back(port);
	pair<int , string> port_host;

	if (_host == "localhost")
		port_host.second = "127.0.0.1";
	else
		port_host.second = _host;
	port_host.first = port;
	ports.push_back(port_host);
}

void Server::set_error_pages(const string& key,const string& value) {
	this->error_pages[key] = value;
}

bool Server::set_server_names(const vector<string>& vec) {
	// this is check duplicate in config file!
	if (!this->server_names.empty())
		return (false);
	for (size_t i = 1; i < vec.size(); i++)
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
	if (global_root[global_root.length() - 1] != '/')
		global_root += '/';
	return (true);
}

// bool Server::set_global_upload_store(const string& upload_stor) {
// 	// to check if the path exist !
// 	// this is check duplicate in config file!
// 	if (!this->global_upload_store.empty())
// 		return (false);
// 	this->global_upload_store = upload_stor;
// 	return (true);
// }

bool Server::set_indexes(const vector<string>& vec) {
	// this is check duplicate in config file!
	if (!this->indexes.empty())
		return (false);
	for (size_t i = 1; i < vec.size(); i++)
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

// bool Server::set_host_name(const string& _host_name) {
// 	if (!host_name.empty())
// 		return false;
// 	if (_host_name == "localhost")
// 		this->host_name = "127.0.0.1";
// 	else
// 		this->host_name = _host_name;
// 	// cout << BOLD_GREEN<< host_name << "\n" <<RESET;
// 	return true;
// }

bool Server::does_not_exist(const string& path) {
	struct stat statbuf;
	return (stat(path.c_str(), &statbuf) != 0); // Returns true if the file does not exist.
}

bool Server::is_a_file(const string& path) {
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf) == 0)
		return S_ISREG(statbuf.st_mode);
	return false; // The path does not exist or is not a regular file.
}

bool Server::check_is_dir(const string& path, int num_server) {

	if (does_not_exist(path)) {
		cerr << BOLD_RED << "Error server" << num_server <<": path => " << path << " does not exist!\n" << RESET;
		return false;
	}
	if (is_a_file(path)) {
		cerr << BOLD_RED << "Error server" << num_server <<": path => " << path << " should not be a file!\n" << RESET;
		return (false);
	} else {
		if (access(path.c_str(), W_OK | X_OK) != 0) {
			cerr << BOLD_RED << "Error server" << num_server <<": You don't have the write permission for: " << path << "\n" << RESET;
			return (false);
		}
	}
	return (true);
}

bool Server::check_attributes_server() {
	static int order_server = 1;
	if (ports.empty()) {
		cerr << BOLD_RED << "Error: server"<< order_server << " => need port!\n" << RESET;
		return (false);
	}
	// if (server_names.empty()) {
	// 	cerr << BOLD_RED << "Error: server"<< order_server << " => need server_name!\n" << RESET;
	// 	return (false);
	// }
	if (global_root.empty()) {
		cerr << BOLD_RED << "Error: server"<< order_server << " => need global root path!\n" << RESET;
		return (false);
	}
	else {
		if (!check_is_dir(global_root, order_server))
			return (false);
	}
	// if (global_upload_store.empty()) {
	// 	cerr << BOLD_RED << "Error: server"<< order_server << " => need global upload store path!\n" << RESET;
	// 	return (false);
	// }
	// else {
	// 	if (!check_is_dir(global_upload_store, order_server))
	// 		return (false);
	// }
	// check each location!
	indexes.push_back("index.html");
	for (size_t i = 0; i < locations.size(); i++)
	{
		locations[i].append_index();
		if (!locations[i].get_root().empty()) {
			// locations[i].set_location_upload_store(global_root + global_upload_store);
			if(!locations[i].check_is_dir(locations[i].get_root(), order_server))
				return (false);
		}
		if (!locations[i].get_location_upload_store().empty()) {
			// locations[i].set_location_upload_store(locations[i].get_root() + global_upload_store);
			if(!locations[i].check_is_dir(locations[i].get_location_upload_store(), order_server))
				return (false);
		}
		if (locations[i].get_methods().at("POST") && locations[i].get_location_upload_store().empty()) {
			cerr << BOLD_RED << "Error server" << order_server << " in a location => POST ALLOWED, NEED UPLOAD STOR!\n" << RESET;
			return false;
		}
	}
	// global_upload_store = global_root + global_upload_store;
	// if (!check_is_dir(global_upload_store, order_server))
	// 	return (false);
	order_server++;
	return (true);
}



void Server::print_server_info(void) const {
    cout << BOLD_YELLOW << "********************* server info *********************" << RESET << endl;
    cout << BG_BLACK;

    cout << BOLD_BLUE << left << setw(20) << "ports " << ": " << BOLD_WHITE << "[";
    for (size_t i = 0; i < ports.size(); i++) {
        if (i > 0)
            cout << ", ";
        cout << ports[i].first << ":_host:" << ports[i].second;
    }
    cout << "]" << endl;

    cout << BOLD_BLUE << left << setw(20) << "server names " << ": " << BOLD_WHITE << "[";
    for (size_t i = 0; i < server_names.size(); i++) {
        if (i > 0)
            cout << ", ";
        cout << server_names[i];
    }
    cout << "]" << endl;

    cout << BOLD_BLUE << left << setw(20) << "indexes " << ": " << BOLD_WHITE << "[";
    for (size_t i = 0; i < indexes.size(); i++) {
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
    // cout << BOLD_BLUE << left << "" << "global_upload_store: " << BOLD_WHITE << global_upload_store << "\n";

    cout << BOLD_BLUE << left << setw(20) << "global_root " << " :" << BOLD_WHITE << global_root << "\n";

    cout << BOLD_BLUE << left << setw(20) <<  "redirection " << ": " << BOLD_WHITE;
    cout << "[" << redirection.first << "]" << " [" << redirection.second << "]\n";

    cout << BOLD_BLUE << left << setw(20) << "auto_index " << ": " << BOLD_WHITE;
    if (autoindex) cout << "on" << endl;
    else cout << "off" << endl;

    for (size_t i = 0; i < locations.size(); i++) {
        cout << BOLD_BLUE << left << setw(20) << "location "  << ": " << BOLD_WHITE << i + 1 << "\n";
        locations[i].print_lacation_info();
    }

    // cout << BOLD_BLUE << left << setw(20) << "host_name " << " :" << BOLD_WHITE << host_name << "\n";
}