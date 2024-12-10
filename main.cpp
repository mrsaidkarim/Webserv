/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skarim <skarim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 12:06:33 by skarim            #+#    #+#             */
/*   Updated: 2024/12/10 20:48:47 by skarim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "const.hpp"
#include "Configuration/Location.hpp"
#include "Configuration/Server.hpp"
#include "Request/HttpRequest.hpp"
#include "WebServ.hpp"

int main(int ac, char **av)
{
    // vector<string> route = {"home", "repo1", "index.html"};
    // vector<string> indexes = {"index.html", "home.html"};
    // bool auto_index = true;
    // string root = "/var/www/example";
    // map<string, bool> methods = {
    //     {"GET", true},
    //     {"POST", true},
    //     {"DELETE", false}
    // };
    // pair<string, string> redirections = {"301", "www.youtube.com"};

    // // Create a Location object
    // Location loc1(route, indexes, auto_index, root, methods, redirections);
    // Location loc2(
    //     {"home", "repo2", "index.html"},
    //     {"home.html"},
    //     false,
    //     "",
    //     {{"GET", true},
    //     {"POST", false},
    //     {"DELETE", false}},
    //     {}
    // );
    // Location loc3(
    //     {"home", "repo3", "index.html"},
    //     {},
    //     false,
    //     "",
    //     {{"GET", true},
    //     {"POST", false},
    //     {"DELETE", false}},
    //     {"301", "www.youtube.com"}
    // );
    // Location.print_lacation_info();

    // vector<int> ports = {8080, 443};
    // vector<string> server_names = {"zechi.com", "www.nigro.com"};
    // long long client_max_body_size = 10485760; // 10 MB
    // vector<Location> locations = {loc1, loc2, loc3};
    // string global_root = "/globalroot";
    // pair<string, string> redirection = {"301", "www.ingtagram.com"};
    // vector<string> indexes2 = {"indexserver.html", "homeserver.html"};
    // bool autoindex2 = true;
    // vector<string> error_pages = {"404.html", "500.html", "403.html"};
    
    // Server server(
    //     ports,
    //     server_names,
    //     client_max_body_size,
    //     locations,
    //     global_root,
    //     redirection,
    //     indexes2,
    //     autoindex2,
    //     error_pages
    // );

    // test run servers
    Location loc1(
        {"home", "repo1", "index.html"},
        {"home.html"},
        false,
        "",
        {{"GET", true},
        {"POST", false},
        {"DELETE", false}},
        {}
    );
    Location loc2(
        {"home", "repo2", "index.html"},
        {},
        false,
        "",
        {{"GET", true},
        {"POST", false},
        {"DELETE", false}},
        {"301", "www.youtube.com"}
    );
    Location loc3(
        {"home", "repo3", "index.html"},
        {},
        false,
        "",
        {{"GET", true},
        {"POST", false},
        {"DELETE", false}},
        {}
    );
    Server server1(
        {8080},
        {"zechi.com", "www.nigro.com"},
        10485760,
        {loc1, loc2},
        "/globalroot1",
        {"301", "www.ingtagram.com"},
        {"indexserver1.html", "homeserver1.html"},
        true,
        {"404.html", "500.html", "403.html"}
    );

    Server server2(
        {7070},
        {"saim.com"},
        10485760,
        {loc3},
        "/globalroot2",
        {"301", "www.maroc.ma"},
        {"indexserver2.html", "homeserver2.html"},
        true,
        {"404.html", "500.html", "403.html"}
    );
    vector<Server> servers = {server1, server2};
    WebServ ws(servers);
    ws.run_servers();

    //
    // server.print_server_info();
	// const char *http_request = 
	// "GET / HTTP/1.1\r\n"
	// "Host: localhost\r\n"
	// "Content-Length: 13\r\n"
	// "transfer: code\r\n"
	// "test: HTTPHELLO\r\n"
	// "\r\n"
	// "Hello, World!";
	// HttpRequest test(http_request);
	// if (test.get_status_code().empty())
	// 	test.display_request();
	// else
	// 	cout << RED << "error: " << test.get_status_code() << "\n" << RESET;
}