/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skarim <skarim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 12:06:33 by skarim            #+#    #+#             */
/*   Updated: 2024/12/01 15:03:26 by skarim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "const.hpp"
#include "Configuration/Location.hpp"
#include "Configuration/Server.hpp"

int main(void)
{
    vector<string> route = {"home", "repo1", "index.html"};
    vector<string> indexes = {"index.html", "home.html"};
    bool auto_index = true;
    string root = "/var/www/example";
    map<string, bool> methods = {
        {"GET", true},
        {"POST", true},
        {"DELETE", false}
    };
    pair<string, string> redirections = {"301", "www.youtube.com"};

    // Create a Location object
    Location loc1(route, indexes, auto_index, root, methods, redirections);
    Location loc2(
        {"home", "repo2", "index.html"},
        {"home.html"},
        false,
        "",
        {{"GET", true},
        {"POST", false},
        {"DELETE", false}},
        {}
    );
    Location loc3(
        {"home", "repo3", "index.html"},
        {},
        false,
        "",
        {{"GET", true},
        {"POST", false},
        {"DELETE", false}},
        {"301", "www.youtube.com"}
    );
    // Location.print_lacation_info();

    vector<int> ports = {8080, 443};
    vector<string> server_names = {"zechi.com", "www.nigro.com"};
    long long client_max_body_size = 10485760; // 10 MB
    vector<Location> locations = {loc1, loc2, loc3};
    string global_root = "/globalroot";
    pair<string, string> redirection = {"301", "www.ingtagram.com"};
    vector<string> indexes2 = {"indexserver.html", "homeserver.html"};
    bool autoindex2 = true;
    vector<string> error_pages = {"404.html", "500.html", "403.html"};
    
    Server server(
        ports,
        server_names,
        client_max_body_size,
        locations,
        global_root,
        redirection,
        indexes2,
        autoindex2,
        error_pages
    );

    server.print_server_info();
}