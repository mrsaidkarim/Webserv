
// #include "const.hpp"
// #include "Configuration/Location.hpp"
// #include "Configuration/Server.hpp"
// #include "Request/HttpRequest.hpp"

// int main(int ac, char **av)
// {
//     vector<string> route = {"home", "repo1", "index.html"};
//     vector<string> indexes = {"index.html", "home.html"};
//     bool auto_index = true;
//     string root = "/var/www/example";
//     map<string, bool> methods = {
//         {"GET", true},
//         {"POST", true},
//         {"DELETE", false}
//     };
//     pair<string, string> redirections = {"301", "www.youtube.com"};

//     // Create a Location object
//     Location loc1(route, indexes, auto_index, root, methods, redirections);
//     Location loc2(
//         {"home", "repo2", "index.html"},
//         {"home.html"},
//         false,
//         "",
//         {{"GET", true},
//         {"POST", false},
//         {"DELETE", false}},
//         {}
//     );
//     Location loc3(
//         {"home", "repo3", "index.html"},
//         {},
//         false,
//         "",
//         {{"GET", true},
//         {"POST", false},
//         {"DELETE", false}},
//         {"301", "www.youtube.com"}
//     );
//     Location.print_lacation_info();

//     vector<int> ports = {8080, 443};
//     vector<string> server_names = {"zechi.com", "www.nigro.com"};
//     long long client_max_body_size = 10485760; // 10 MB
//     vector<Location> locations = {loc1, loc2, loc3};
//     string global_root = "/globalroot";
//     pair<string, string> redirection = {"301", "www.ingtagram.com"};
//     vector<string> indexes2 = {"indexserver.html", "homeserver.html"};
//     bool autoindex2 = true;
//     vector<string> error_pages = {"404.html", "500.html", "403.html"};
    
//     Server server(
//         ports,
//         server_names,
//         client_max_body_size,
//         locations,
//         global_root,
//         redirection,
//         indexes2,
//         autoindex2,
//         error_pages
//     );

//     // server.print_server_info();
// 	const char *http_request = 
// 	"GET / HTTP/1.1\r\n"
// 	"Host: localhost\r\n"
// 	"Content-Length: 13\r\n"
// 	"transfer: code\r\n"
// 	"test: HTTPHELLO\r\n"
// 	"\r\n"
// 	"Hello, World!";
// 	HttpRequest test(http_request);
// 	if (test.get_status_code().empty())
// 		test.display_request();
// 	else
// 		cout << RED << "error: " << test.get_status_code() << "\n" << RESET;
// }


#include "WebServ.hpp"
#include "Configuration/Location.hpp"
#include "Response/HttpResponse.hpp"
#include "Configuration/Server.hpp"
#include "Request/HttpRequest.hpp"


Server server_init1() {
    vector<string> route = {"youtube"};
    vector<string> indexes = {};
    bool auto_index = false;
    string root = "";
    map<string, bool> methods = {
        {"GET", true},
        {"POST", true},
        {"DELETE", false}
    };
    pair<string, string> redirections = {"301", "www.youtube.com"};

    // Create a Location object
    Location loc1(route, indexes, auto_index, root, methods, redirections);
    Location loc2(
        {"A2", "B2", "C2"},
        {"home.html"},
        true,
        "/Volumes/TOSHIBA/www/a2",
        {{"GET", true},
        {"POST", false},
        {"DELETE", false}},
        {}
    );
    Location loc3(
        {"A3", "B3", "C3"},
        {},
        false,
        "/Volumes/TOSHIBA/www/a3",
        {{"GET", true},
        {"POST", false},
        {"DELETE", false}},
        {}
    );
    Location loc4(
        {"A4", "B4", "C4"},
        {},
        true,
        "/Volumes/TOSHIBA/www/a4",
        {{"GET", true},
        {"POST", false},
        {"DELETE", false}},
        {}
    );
    // Location.print_lacation_info();

    vector<int> ports = {8080};
    vector<string> server_names = {"zechi.com", "www.nigro.com"};
    long long client_max_body_size = -1;
    vector<Location> locations = {loc1, loc2, loc3, loc4};
    string global_root = "/Volumes/TOSHIBA/www";
    // pair<string, string> redirection = {"301", "https://auth.42.fr"};
    pair<string, string> redirection = {};
    vector<string> indexes2 = {"vvv.mp4"};
    bool autoindex2 = false;
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

    return (server);
}


int main() {
    signal (SIGPIPE, SIG_IGN);
    cout.setf(ios::unitbuf);
    Server server1 = server_init1();
    Server server2 = server_init1();
    Server server3 = server_init1();
    WebServ webserv;

    server1.print_server_info();
    webserv.create_sockets(server1);
    webserv.set_servers({server1, server2, server3});
    webserv.run();
}