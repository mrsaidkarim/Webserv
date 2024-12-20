/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelabbas <zelabbas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 11:24:19 by skarim            #+#    #+#             */
/*   Updated: 2024/12/20 12:01:24 by zelabbas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location()
{
	methods["GET"] = true;
	methods["POST"] = false;
	methods["DELETE"] = false;
	auto_index = false;
	methods_set = false;
	auto_index_set = false;
	cout << "constructor location\n";
}

Location::Location(const vector<string> &route, const vector<string> &indexes, bool auto_index,
                const string &root, const map<string, bool> &methods,
                const pair<string, string> &redirections): route(route), indexes(indexes), auto_index(auto_index), root(root), methods(methods), redirections(redirections)
{
}
        // Location(string file_path); here do your shit alparser nigro o zid setters;
Location::~Location()
{
	// cout << "destructor location\n";
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
	// if (!this->route.empty())
	// 	return (false);
	for (int i = 1; i < route.size() - 1; i++)
	{
		this->route.push_back(route[i]);
	}
    // this->route = route;
    return (true);
}

bool Location::set_indexes(const vector<string> &indexes)
{
	// this mean duplicate in config file
	if (!this->indexes.empty())
		return (false);
	for (int i = 1; i < indexes.size(); i++)
	{
		this->indexes.push_back(indexes[i]);
	}
    // this->indexes = indexes;
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
		if (access(path.c_str(), W_OK | X_OK) != 0) {
			cerr << BOLD_RED << "Error server" << num_server << " in location: You don't have the write permission for: " << path << "\n" << RESET;
			return (false);
		}
	}
	return (true);
}

void Location::print_lacation_info() const {
    cout << BG_WHITE;

    cout << BOLD_BLUE << "\t" << "route       : ";
    for (unsigned long i = 0; i < route.size(); i++) {
        cout << route[i] << " ";
        // if (i < route.size() - 1)
        //     cout << "/";
    }
    // for(unsigned ) << BOLD_BLACK << route << " \n";
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
    auto it = methods.find("GET");
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

    cout << RESET;

}