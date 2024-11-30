/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skarim <skarim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 11:24:19 by skarim            #+#    #+#             */
/*   Updated: 2024/11/30 12:01:47 by skarim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location()
{
    
}

Location::Location(const string &route, const vector<string> &indexes, bool auto_index,
                const string &root, const map<string, bool> &methods,
                const pair<string, string> &redirections): route(route), indexes(indexes), auto_index(auto_index), root(root), methods(methods), redirections(redirections)
{
}
        // Location(string file_path); here do your shit alparser nigro o zid setters;
Location::~Location()
{
    
}

const string &Location::get_route(void) const
{
    return (route);
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

bool Location::set_route(const string &route)
{
    this->route = route;
    return (true);
}

bool Location::set_indexes(const vector<string> &indexes)
{
    this->indexes = indexes;
    return (true);
}

bool Location::set_auto_index(bool auto_index)
{
    this->auto_index = auto_index;
    return (true);
}

bool Location::set_root(const string &root)
{
    this->root = root;
    return (true);
}

bool Location::set_methods(const map<string, bool> &methods)
{
    this->methods = methods;
    return (true);
}

bool Location::set_redirections(const pair<string, string> &redirections)
{
    this->redirections = redirections;
    return (true);
}


void Location::print_lacation_info() const {
    cout << BG_WHITE;

    cout << BOLD_BLUE << "\t" << "route       : " << BOLD_BLACK << route << " \n";

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