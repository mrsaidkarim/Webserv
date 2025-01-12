#include "WebServ.hpp"
#include "Configuration/Location.hpp"
#include "Response/HttpResponse.hpp"
#include "Configuration/Server.hpp"
#include "Request/HttpRequest.hpp"


// this function initializes a server object
// with some dummy data
// this is just for testing purposes
// all those data should be read from the config file
Server server_init1() {
    vector<string> route = {"youtube"};
    vector<string> indexes = {"index.html"}; // always have index.html in the end of indexes
    bool auto_index = false;
    string root = "";
    map<string, bool> methods = {
        {"GET", true},
        {"POST", true},
        {"DELETE", true}
    };
    map<string, string> cgi1 = {
        {"php", ""},
        {"py", ""},
        {"js", ""}
    };
    map<string, string> cgi2 = {
        {"php", ""},
        {"py", ""},
        {"js", ""}
    };
    map<string, string> cgi3 = {
        {"php", ""},
        {"py", ""},
        {"js", ""}
    };
    map<string, string> cgi4 = {
        {"php", "/usr/bin/zelabasss"}, {"py", "/usr/bin/python3"}, {"js", "/usr/local/bin/node"}
    };
    pair<string, string> redirections = {"301", "www.youtube.com"};

    // Create a Location object
    Location loc1(route, indexes, auto_index, root, methods, redirections, cgi1);
    Location loc2(
        {"cookie"},
        {"send_form.html"},
        false,
        "/Volumes/TOSHIBA/www/", // it was something /Volumes/Toshiba.....
        {{"GET", true},
        {"POST", true},
        {"DELETE", false}}, // it was false
        {}, cgi4
    );
    Location loc3(
        {"cookie2"},
        {"cookie2.py"},
        false,
        "/Volumes/TOSHIBA/www/",
        {{"GET", true},
        {"POST", true},
        {"DELETE", false}}, // it was false
        {}, cgi4
    );
    Location loc4(
        {"cgi-tiriri"},
        {"index.html"},
        true,
        "/Volumes/TOSHIBA/www/",
        {{"GET", true},
        {"POST", true},
        {"DELETE", false}}, // it was false
        {}, cgi4
    );
    // Location.print_lacation_info();

    vector<int> ports = {8080};
    vector<string> server_names = {"zechi.com", "www.nigro.com"};
    long long client_max_body_size = -1;
    vector<Location> locations = {loc1, loc2, loc3, loc4};
    string global_root = "/Volumes/TOSHIBA/www/";
    // pair<string, string> redirection = {"301", "https://auth.42.fr"};
    pair<string, string> redirection = {"", ""};
    vector<string> indexes2 = {"index.html"};
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

    return (server);
}
// Server server_init2() {
//     vector<string> route = {"youtube"};
//     vector<string> indexes = {"index.html"}; // always have index.html in the end of indexes
//     bool auto_index = false;
//     string root = "";
//     map<string, bool> methods = {
//         {"GET", true},
//         {"POST", true},
//         {"DELETE", false}
//     };
//     pair<string, string> redirections = {"301", "www.youtube.com"};

//     // Create a Location object
//     Location loc1(route, indexes, auto_index, root, methods, redirections);
//     Location loc2(
//         {"A2", "B2", "C2"},
//         {"home.html", "index.html"},
//         true,
//         "/Volumes/TOSHIBA/www/a2",
//         {{"GET", true},
//         {"POST", false},
//         {"DELETE", false}},
//         {}
//     );
//     Location loc3(
//         {"A3", "B3", "C3"},
//         {"index.html"},
//         false,
//         "/Volumes/TOSHIBA/www/a3",
//         {{"GET", true},
//         {"POST", false},
//         {"DELETE", false}},
//         {}
//     );
//     Location loc4(
//         {"A4", "B4", "C4"},
//         {"index.html"},
//         true,
//         "/Volumes/TOSHIBA/www/a4",
//         {{"GET", true},
//         {"POST", false},
//         {"DELETE", false}},
//         {}
//     );
//     // Location.print_lacation_info();

//     vector<int> ports = {8000, 8001};
//     vector<string> server_names = {"zechi.com", "www.nigro.com"};
//     long long client_max_body_size = -1;
//     vector<Location> locations = {loc1, loc2, loc3, loc4};
//     string global_root = "/Volumes/TOSHIBA/www";
//     // pair<string, string> redirection = {"301", "https://auth.42.fr"};
//     pair<string, string> redirection = {"", ""};
//     vector<string> indexes2 = {"video1.mp4"};
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

//     return (server);
// }
// Server server_init3() {
//     vector<string> route = {"youtube"};
//     vector<string> indexes = {"index.html"}; // always have index.html in the end of indexes
//     bool auto_index = false;
//     string root = "";
//     map<string, bool> methods = {
//         {"GET", true},
//         {"POST", true},
//         {"DELETE", false}
//     };
//     pair<string, string> redirections = {"301", "www.youtube.com"};

//     // Create a Location object
//     Location loc1(route, indexes, auto_index, root, methods, redirections);
//     Location loc2(
//         {"A2", "B2", "C2"},
//         {"home.html", "index.html"},
//         true,
//         "/Volumes/TOSHIBA/www/a2",
//         {{"GET", true},
//         {"POST", false},
//         {"DELETE", false}},
//         {}
//     );
//     Location loc3(
//         {"A3", "B3", "C3"},
//         {"index.html"},
//         false,
//         "/Volumes/TOSHIBA/www/a3",
//         {{"GET", true},
//         {"POST", false},
//         {"DELETE", false}},
//         {}
//     );
//     Location loc4(
//         {"A4", "B4", "C4"},
//         {"index.html"},
//         true,
//         "/Volumes/TOSHIBA/www/a4",
//         {{"GET", true},
//         {"POST", false},
//         {"DELETE", false}},
//         {}
//     );
//     // Location.print_lacation_info();

//     vector<int> ports = {8002, 8003, 8004};
//     vector<string> server_names = {"zechi.com", "www.nigro.com"};
//     long long client_max_body_size = -1;
//     vector<Location> locations = {loc1, loc2, loc3, loc4};
//     string global_root = "/Volumes/TOSHIBA/www";
//     // pair<string, string> redirection = {"301", "https://auth.42.fr"};
//     pair<string, string> redirection = {"", ""};
//     vector<string> indexes2 = {"book.pdf"};
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

//     return (server);
// }


int main() {
    signal (SIGPIPE, SIG_IGN); // ignore broken pipe signal from client
    cout.setf(ios::unitbuf); // flushes the buffer after every output operation
    // create servers objects
    Server server1 = server_init1();
    Server server2 = server_init1();
    Server server3 = server_init1();

    vector<Server> servers = {server1};

    // create WebServ object
    WebServ webserv(servers);

    // run the servers
    webserv.run_servers();
}
