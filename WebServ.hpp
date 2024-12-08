#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "const.hpp"
#include "Request/HttpRequest.hpp"
#include "Response/HttpResponse.hpp"
#include "Configuration/Server.hpp"

class WebServ {
    private:
        vector<Server>      servers;
        int                 serv_fd;
        struct sockaddr_in  serv_addr;
        struct sockaddr_in  serv_communication_addr;
        int                 serv_socket_communication_fd;
        int                 serv_http_method;
        int                 serv_addr_size;
        string              serv_request_buffer;
        vector<string> serv_request_first_line;
        map<string, string> serv_request_header;

    public:
        WebServ();
        ~WebServ();

        void create_sockets(Server &);
        void run(); // run all servers;
        // void listen(); // listen to all requests and do action;
        void close_sockets(); 

        Server    host_server_name(const HttpRequest &);
        // SETTERS
        void set_servers(const vector<Server>);

};

#endif
