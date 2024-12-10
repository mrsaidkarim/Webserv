#include "WebServ.hpp"
#include "Configuration/Server.hpp"
#include "Request/HttpRequest.hpp"
#include "Response/HttpResponse.hpp"
#include "const.hpp"
#include <unistd.h>

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
Server    WebServ::host_server_name(const map<string, string> header) {
    if (servers.empty()) {
        cerr << "no servers setup\n" << "\n";
        exit(1);
    }
    Server  server;
    if (header.find("Host") == header.end()) {
        server = servers[0];
        return (server);
    }
    string host = header.find("Host")->second;
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


// void WebServ::run() {
//     // Create the kqueue instance
//     int kq = kqueue();
//     if (kq == -1) {
//         cerr << "Error: Failed to create kqueue" << endl;
//         exit(EXIT_FAILURE);
//     }

//     // Configure server socket for monitoring
//     struct kevent changes[1];
//     EV_SET(&changes[0], serv_fd, EVFILT_READ, EV_ADD, 0, 0, nullptr);
//     struct timespec timeout = {0, 0}; // Zero timeout for non-blocking mode
//     if (kevent(kq, changes, 1, nullptr, 0, &timeout) == -1) {
//         cerr << "Error: Failed to add event to kqueue" << endl;
//         close(kq); // Cleanup before exit
//         exit(EXIT_FAILURE);
//     }

//     // Set server socket to non-blocking mode
//     int flags = fcntl(serv_fd, F_GETFL, 0);
//     if (flags == -1) {
//         cerr << "Error: Failed to get server socket flags" << endl;
//         close(kq);
//         exit(EXIT_FAILURE);
//     }

//     if (fcntl(serv_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
//         cerr << "Error: Failed to set server socket to non-blocking mode" << endl;
//         close(kq);
//         exit(EXIT_FAILURE);
//     }

//     struct kevent events[1];

//     while (true) {
//         // Wait for events on kqueue
//         int nev = kevent(kq, nullptr, 0, events, 1, nullptr);
//         if (nev == -1) {
//             cerr << "Error: Failed to wait for event on kqueue" << endl;
//             break;
//         }

//         if (nev > 0) {
//             int fd = events[0].ident;

//             // Accept new connection
//             serv_socket_communication_fd = accept(fd, (struct sockaddr *)&serv_communication_addr, (socklen_t *)&serv_addr_size);
//             if (serv_socket_communication_fd > 0) {
//                 // Read incoming request
//                 serv_request_buffer = string(BUFFER_SIZE2, '\0');
//                 ssize_t bytes_received = recv(serv_socket_communication_fd, &serv_request_buffer[0], BUFFER_SIZE2, MSG_DONTWAIT);
//                 if (bytes_received > 0) {
//                     cout << BOLD_GREEN << "Request buffer: " << serv_request_buffer << "\n" << RESET;

//                     // Parse and handle the request
//                     HttpRequest* request = new HttpRequest(serv_request_buffer, serv_socket_communication_fd);
//                     Server server = host_server_name(request->get_header());
//                     request->set_server(server);
//                     requests.push_back(request);
//                 }
//                 for (auto it = requests.begin(); it != requests.end();) {
//                     HttpRequest* request = *it;
//                     HttpResponse response(request);
//                     response.serv();

//                     if (request->get_is_complete()) {
//                         delete request;
//                         it = requests.erase(it); // Remove completed request
//                     } else {
//                         ++it;
//                     }
//                 }

//             } else {
//                 // for (auto it = requests.begin(); it != requests.end();) {
//                 //     HttpRequest* request = *it;
//                 //     HttpResponse response(request);
//                 //     response.serv();

//                 //     if (request->get_is_complete()) {
//                 //         delete request;
//                 //         it = requests.erase(it); // Remove completed request
//                 //     } else {
//                 //         ++it;
//                 //     }
//                 // }
//             }
//         }

//         // Process all queued requests
//         for (auto it = requests.begin(); it != requests.end();) {
//             HttpRequest* request = *it;
//             HttpResponse response(request);
//             response.serv();

//             if (request->get_is_complete()) {
//                 delete request;
//                 it = requests.erase(it); // Remove completed request
//             } else {
//                 ++it;
//             }
//         }

//         // cout << "Active requests: " << requests.size() << "\n";
//     }

//     // Cleanup
//     close(kq);
// }



void WebServ::run() {
    // Create the kqueue instance
    int kq = kqueue();
    if (kq == -1) {
        cerr << "Error: Failed to create kqueue" << endl;
        exit(EXIT_FAILURE);
    }

    // Configure server socket for monitoring
    struct kevent changes[1];
    EV_SET(&changes[0], serv_fd, EVFILT_READ, EV_ADD, 0, 0, nullptr);
    struct timespec timeout = {0, 0}; // Zero timeout for non-blocking mode
    if (kevent(kq, changes, 1, nullptr, 0, &timeout) == -1) {
        cerr << "Error: Failed to add event to kqueue" << endl;
        close(kq);
        exit(EXIT_FAILURE);
    }

    // Set server socket to non-blocking mode
    // int flags = fcntl(serv_fd, F_GETFL, 0);
    // if (flags == -1) {
    //     cerr << "Error: Failed to get server socket flags" << endl;
    //     close(kq);
    //     exit(EXIT_FAILURE);
    // }

    // if (fcntl(serv_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
    //     cerr << "Error: Failed to set server socket to non-blocking mode" << endl;
    //     close(kq);
    //     exit(EXIT_FAILURE);
    // }

    struct kevent events[64];
    unordered_map<int, HttpResponse*> client_responses; // Track responses by client fd

    while (true) {
        // Wait for events on kqueue
        int nev = kevent(kq, nullptr, 0, events, 64, nullptr);
        if (nev == -1) {
            cerr << "Error: Failed to wait for event on kqueue" << endl;
            break;
        }

        for (int i = 0; i < nev; i++) {
            int fd = events[i].ident;

            if (events[i].filter == EVFILT_READ && fd == serv_fd) {
                // Accept new connection
                serv_socket_communication_fd = accept(fd, (struct sockaddr *)&serv_communication_addr, (socklen_t *)&serv_addr_size);
                if (serv_socket_communication_fd > 0) {
                    cout << BOLD_RED << "new connection\n" << RESET;
                    // Add client socket to kqueue for read events
                    EV_SET(&changes[0], serv_socket_communication_fd, EVFILT_READ, EV_ADD, 0, 0, nullptr);
                    kevent(kq, changes, 1, nullptr, 0, nullptr);
                }
            } else if (events[i].filter == EVFILT_READ) {
                // Read incoming request
                serv_request_buffer = string(BUFFER_SIZE2, '\0');
                ssize_t bytes_received = recv(fd, &serv_request_buffer[0], BUFFER_SIZE2, MSG_DONTWAIT);
                if (bytes_received > 0) {
                    HttpRequest* request = new HttpRequest(serv_request_buffer, fd);
                    Server server = host_server_name(request->get_header());
                    request->set_server(server);
                    HttpResponse* response = new HttpResponse(request);

                    // Generate the response and prepare for writing
                    response->serv();
                    client_responses[fd] = response;

                    // Add client socket to kqueue for write events
                    EV_SET(&changes[0], fd, EVFILT_WRITE, EV_ADD, 0, 0, nullptr);
                    kevent(kq, changes, 1, nullptr, 0, nullptr);
                } else if (bytes_received == 0) {
                    // Client disconnected
                    close(fd);
                }
            } else if (events[i].filter == EVFILT_WRITE) {
                // Send chunked response
                HttpResponse* response = client_responses[fd];
                if (response) {
                    response->serv();
                    if (response->get_request()->get_is_complete()) {
                        // All chunks sent, clean up
                        delete response;
                        client_responses.erase(fd);

                        EV_SET(&changes[0], fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
                        kevent(kq, changes, 1, nullptr, 0, nullptr);
                        close(fd);
                    }
                }
            }
        }
    }

    // Cleanup
    for (auto& [fd, response] : client_responses) {
        delete response;
        close(fd);
    }
    close(kq);
}


void WebServ::set_servers(vector<Server> servers) {
    this->servers = servers;
}


void WebServ::close_sockets() {
    close(serv_fd);
}