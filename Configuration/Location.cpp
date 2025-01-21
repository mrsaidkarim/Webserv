/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skarim <skarim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 11:24:19 by skarim            #+#    #+#             */
/*   Updated: 2024/12/01 15:00:20 by skarim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"
#include <cstddef>

Location::Location()
{
	methods["GET"] = true;
	methods["POST"] = false;
	methods["DELETE"] = false;
	auto_index = false;
	methods_set = false;
	auto_index_set = false;
	cgi_extension["php"] = "";
	cgi_extension["py"] = "";
	cgi_extension["sh"] = "";
}

Location::~Location()
{
    
}

const vector<string> &Location::get_route(void) const
{
    return (route);
}

const string& Location::get_location_upload_store(void) const {
	return (location_upload_store);
}

const vector<string> &Location::get_indexes(void) const
{
    return (indexes);
}

bool Location::get_auto_index(void) const
{
    return (auto_index);
}

const string &Location::get_root(void) const
{
    return (root);
}

const map<string, bool> &Location::get_methods(void) const
{
    return (methods);
}

const pair<string, string> &Location::get_redirections(void) const
{
    return (redirections);
}

bool Location::set_route(const vector<string> &route)
{
	for (size_t i = 0; i < route.size(); i++)
	{
		this->route.push_back(route[i]);
	}
	if (this->route.empty())
		this->route.push_back("/");
    return (true);
}

bool Location::set_indexes(const vector<string> &indexes)
{
	// this mean duplicate in config file
	if (!this->indexes.empty())
		return (false);
	for (size_t i = 1; i < indexes.size(); i++)
	{
		this->indexes.push_back(indexes[i]);
	}
    return (true);
}

bool Location::set_auto_index(bool auto_index)
{
	// this mean duplicate in config file
	if (auto_index_set)
		return (false);
    this->auto_index = auto_index;
	auto_index_set = true;
    return (true);
}

bool Location::set_root(const string &root)
{
	// this mean duplicate in config file
	if (!this->root.empty())
		return (false);
    this->root = root;
	if (this->root[this->root.length() - 1] != '/')
		this->root += '/';
    return (true);
}

bool Location::set_methods(const map<string, bool> &methods)
{
	// this mean duplicate in config file
	if (methods_set)
		return (false);
    this->methods = methods;
	methods_set = true;
    return (true);
}

bool Location::set_location_upload_store(const string& upload_store) {
	// this mean duplicate in config file
	if (!location_upload_store.empty())
		return (false);
	this->location_upload_store = upload_store;
	if (this->location_upload_store[this->location_upload_store.length() - 1] != '/')
		this->location_upload_store += '/';
	return (true);
}

bool Location::set_redirections(const pair<string, string> &redirections)
{
	// this mean duplicate in config file
	if (!this->redirections.first.empty() && !this->redirections.second.empty())
		return (false);
    this->redirections = redirections;
    return (true);
}

bool Location::does_not_exist(const string& path) {
	struct stat statbuf;
	return (stat(path.c_str(), &statbuf) != 0); // Returns true if the file does not exist.
}

bool Location::is_a_file(const string& path) {
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf) == 0)
		return S_ISREG(statbuf.st_mode);
	return false; // The path does not exist or is not a regular file.
}

bool Location::check_is_dir(const string& path, int num_server) {

	if (does_not_exist(path)) {
		cerr << BOLD_RED << "Error server" << num_server <<" in location: path => " << path << " does not exist!\n" << RESET;
		return false;
	}
	if (is_a_file(path)) {
		cerr << BOLD_RED << "Error server" << num_server << " in location: path => " << path << " should not be a file!\n" << RESET;
		return (false);
	} else {
		if (access(path.c_str(), R_OK | X_OK) != 0) {
			cerr << BOLD_RED << "Error server" << num_server << " in location: You don't have the write permission for: " << path << "\n" << RESET;
			return (false);
		}
	}
	return (true);
}

bool Location::check_binary_file(const string& path) {
	if (does_not_exist(path)) {
		cerr << BOLD_RED << "path does not exist!" << "\n" << RESET;
		return (false);
	}
	if (!is_a_file(path)) {
		cerr << BOLD_RED << "should be a path to binary file!" << "\n" << RESET;
		return (false);
	}
	return (true);
}

bool Location::set_cgi_extension(const vector<string>& vec) {

	if (vec.size() != 3)
		return (false);
	if (vec[1] != "php" && vec[1] != "py" && vec[1] != "sh")
		return (false);
	if (!cgi_extension[vec[1]].empty())
		return (false);
	if (!check_binary_file(vec[2]))
		return (false);
	cgi_extension[vec[1]] = vec[2];
	return (true);
}

const string &Location::get_path_cgi(const string &key) const {
    return (cgi_extension.find(key)->second);
}

void Location::append_index(void){
	indexes.push_back("index.html");
}


void Location::print_lacation_info() const {
    cout << BG_WHITE;

    cout << BOLD_BLUE << "\t" << "route       : ";
    for (unsigned long i = 0; i < route.size(); i++) {
        cout << route[i] << " ";
    }
    cout << "\n";

    cout << BOLD_BLUE << "\t" << "index       : " << BOLD_BLACK << "[";
    for (unsigned long i = 0; i < indexes.size(); i++) {
        if (i > 0)
            cout << ", ";
        cout << indexes[i];
    }
    cout << "] " << endl;

    cout << BOLD_BLUE << "\t" << "auto_index  : " << BOLD_BLACK;
    if (auto_index) cout << "on " << " \n";
    else cout << "off " << " \n";

    cout << BOLD_BLUE << "\t" << "root        : " << BOLD_BLACK << root << " \n";
    cout << BOLD_BLUE << "\t" << "location_upload_store        : " << BOLD_BLACK << location_upload_store << " \n";

    cout << BOLD_BLUE << "\t" <<  "methods     : \n" << BOLD_BLACK;
    cout << "\t      GET   : ";
    map<string, bool>::const_iterator it = methods.find("GET");
    if (it == methods.end() || it->second == false)
        cout << "false \n";
    else
        cout << "true \n";
    cout << "\t      POST  : ";
    it = methods.find("POST");
    if (it == methods.end() || it->second == false)
        cout << "false \n";
    else
        cout << "true \n";
    cout << "\t      DELETE: ";
    it = methods.find("DELETE");
    if (it == methods.end() || it->second == false)
        cout << "false \n";
    else
        cout << "true \n";

    cout << BOLD_BLUE << "\t" <<  "redirection : " << BOLD_BLACK;
    cout << "[" << redirections.first << "]" << " [" << redirections.second << "] \n";

	cout << BOLD_BLUE << "\t" << "cgi_extension : \n" << BOLD_BLACK;
	for (map<string, string>::const_iterator it = cgi_extension.begin(); it != cgi_extension.end() ; it++)
	{
		cout << "\t\t" <<it->first << " : " << it->second << "\n";
	}
	cout << RESET;
}
