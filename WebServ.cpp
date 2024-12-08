#include "WebServ.hpp"
#include "Configuration/Server.hpp"
#include "Request/HttpRequest.hpp"
#include "Response/HttpResponse.hpp"
#include "const.hpp"

WebServ::WebServ() {
    cout << "WebServ Constructor Called\n";
    serv_addr_size = sizeof(serv_addr);
}

WebServ::~WebServ() {
    cout << "WebServ Destructor Called\n";
}



void WebServ::create_sockets(Server &server) {
    serv_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_fd < 0) {
        cerr << "socket failed" << endl;
        exit(1);
    }

    cout << GREEN << "$$$ socket created\n" << RESET;

    const int enable = 1;
    if (setsockopt(serv_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        cerr << "setsockopt (SO_REUSEADDR) failed" << endl;
        exit(1);
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server.get_ports()[0]);
    serv_addr.sin_addr.s_addr =  htonl(INADDR_ANY);

    if (bind(serv_fd, (struct sockaddr *)&serv_addr, (socklen_t)serv_addr_size) < 0) {
        cerr << "bind failed\n";
        exit(1);
    }

    cout << GREEN << "$$$ bind works\n" << RESET;

    listen(serv_fd, 99);

    cout << BG_WHITE << BOLD_GREEN << "server is running on port " << server.get_ports()[0] << " ..." << RESET << "\n" ;
}

// search for server that has server_name == Host
Server    WebServ::host_server_name(const HttpRequest &request) {
    if (servers.empty()) {
        cerr << "no servers setup\n" << "\n";
        exit(1);
    }
    Server  server;
    if (request.get_header().find("Host") == request.get_header().end()) {
        server = servers[0];
        return (server);
    }
    string host = request.get_header().find("Host")->second;
    bool   found = false;
    for (int i = 0; i < servers.size(); i++) {
        for (int j = 0; j < servers[i].get_server_names().size(); j++) {
            if (servers[i].get_server_names()[j] == host) {
                server = servers[i];
                found = true;
            }
        }
    }
    
    // if host does not match any server name
    // then the first server get the service job
    if (!found)
        server = servers[0];
    return (server);
}


void WebServ::run() {
    while (1) {
        serv_request_buffer = string(BUFFER_SIZE, '\0');

        serv_socket_communication_fd = accept(serv_fd, (struct sockaddr *)&serv_communication_addr, (socklen_t *)&serv_addr_size);
        if (serv_socket_communication_fd < 0) {
            cerr << "accept failed" << endl;
            exit(1);
        }

        read(serv_socket_communication_fd, &serv_request_buffer[0], BUFFER_SIZE);
        cout << BOLD_GREEN << "request buffer: " << serv_request_buffer << "\n" << RESET;
        HttpRequest request(serv_request_buffer);
        request.display_request();
        Server  server = host_server_name(request);
        HttpResponse response(request, server, serv_socket_communication_fd);
        response.serv();
        close(serv_socket_communication_fd);
    }
}

void WebServ::set_servers(vector<Server> servers) {
    this->servers = servers;
}


void WebServ::close_sockets() {

}