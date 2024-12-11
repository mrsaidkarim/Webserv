// #include <iostream>
// #include <cstring>
// #include <unistd.h>
// #include <arpa/inet.h>
// #include <sys/event.h> // For kqueue
// #include <sys/socket.h>

// #define MAX_CLIENTS 5

// void runServers(int port1, int port2) {
// 	int server_fd1, server_fd2;
// 	sockaddr_in address1, address2;

// 	// Create the first socket
// 	server_fd1 = socket(AF_INET, SOCK_STREAM, 0);
// 	if (server_fd1 == -1) {
// 		perror("Socket creation failed for port 1");
// 		return;
// 	}
// 	address1.sin_family = AF_INET;
// 	address1.sin_addr.s_addr = INADDR_ANY;
// 	address1.sin_port = htons(port1);

// 	if (bind(server_fd1, (sockaddr*)&address1, sizeof(address1)) < 0) {
// 		perror("Bind failed for port 1");
// 		close(server_fd1);
// 		return;
// 	}
// 	if (listen(server_fd1, MAX_CLIENTS) < 0) {
// 		perror("Listen failed for port 1");
// 		close(server_fd1);
// 		return;
// 	}

// 	// Create the second socket
// 	server_fd2 = socket(AF_INET, SOCK_STREAM, 0);
// 	if (server_fd2 == -1) {
// 		perror("Socket creation failed for port 2");
// 		close(server_fd1);
// 		return;
// 	}
// 	address2.sin_family = AF_INET;
// 	address2.sin_addr.s_addr = INADDR_ANY;
// 	address2.sin_port = htons(port2);

// 	if (bind(server_fd2, (sockaddr*)&address2, sizeof(address2)) < 0) {
// 		perror("Bind failed for port 2");
// 		close(server_fd1);
// 		close(server_fd2);
// 		return;
// 	}
// 	if (listen(server_fd2, MAX_CLIENTS) < 0) {
// 		perror("Listen failed for port 2");
// 		close(server_fd1);
// 		close(server_fd2);
// 		return;
// 	}

// 	std::cout << "Servers running on ports " << port1 << " and " << port2 << "...\n";

// 	// Create a kqueue instance
// 	int kq = kqueue();
// 	if (kq == -1) {
// 		perror("kqueue creation failed");
// 		close(server_fd1);
// 		close(server_fd2);
// 		return;
// 	}

// 	// Register the server sockets with kqueue
// 	struct kevent ev_set[2];
// 	EV_SET(&ev_set[0], server_fd1, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
// 	EV_SET(&ev_set[1], server_fd2, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
// 	if (kevent(kq, ev_set, 2, nullptr, 0, nullptr) == -1) {
// 		perror("kevent registration failed");
// 		close(server_fd1);
// 		close(server_fd2);
// 		close(kq);
// 		return;
// 	}

// 	// std::cout << "the sockets is : " << server_fd1 << "," << server_fd2 << "\n";

// 	// Event loop
// 	struct kevent ev_list[2]; // To store triggered events
// 	// for (int i = 0; i < 2; i++)
// 	// {
// 	// 	std::cout << "the indent is : " << ev_list[i].ident << "\n";
// 	// }
	
// 	while (true) {
// 		int nev = kevent(kq, nullptr, 0, ev_list, 2, nullptr);
// 		if (nev < 0) {
// 			perror("kevent error");
// 			break;
// 		}
// 		for (int i = 0; i < nev; i++) {
// 			int fd = ev_list[i].ident; // File descriptor with activity
// 			std::cout << "the ident is :" << ev_list[i].ident << "\n";

// 			if (fd == server_fd1) {
// 				// Handle activity on server_fd1
// 				sockaddr_in client_addr;
// 				socklen_t client_len = sizeof(client_addr);
// 				int client_fd = accept(server_fd1, (sockaddr*)&client_addr, &client_len);
// 				if (client_fd < 0) {
// 					perror("Accept failed on port 1");
// 				} else {
// 					std::cout << "Client connected on port " << port1 << "\n";
// 					char Request[1024];
// 					size_t readbytes = recv(client_fd, Request, 1024, 0);
// 					std::cout << "the request is: " << Request << "\n";
// 					for (int i = 0; i < readbytes; i++)
// 					{
// 						std::cout << Request[i];
// 					}
// 					std::string http_response = 
// 						"HTTP/1.1 200 OK\r\n"
// 						"Content-Type: text/html\r\n"
// 						"Content-Length: 45\r\n"
// 						"\r\n"
// 						"<html><body><h1>Hello, World1!</h1></body></html>";
// 					send(client_fd, http_response.c_str(), strlen(http_response.c_str()), 0);
// 					close(client_fd);
// 				}
// 			} else if (fd == server_fd2) {
// 				// Handle activity on server_fd2
// 				sockaddr_in client_addr;
// 				socklen_t client_len = sizeof(client_addr);
// 				int client_fd = accept(server_fd2, (sockaddr*)&client_addr, &client_len);
// 				if (client_fd < 0) {
// 					perror("Accept failed on port 2");
// 				} else {
// 					std::cout << "Client connected on port " << port2 << "\n";
// 					char Request[1024];
// 					size_t readbytes = recv(client_fd, Request, 1024, 0);
// 					Request[readbytes] = '\0';
// 					std::cout << "the request is : " << Request << "\n";
// 					const char *http_response = 
// 						"HTTP/1.1 200 OK\r\n"
// 						"Content-Type: text/html\r\n"
// 						"Content-Length: 45\r\n"
// 						"\r\n"
// 						"<html><body><h1>Hello, World2!</h1></body></html>";
// 					send(client_fd, http_response, strlen(http_response), 0);
// 					close(client_fd);
// 				}
// 			}
// 		}
// 	}

// 	close(server_fd1);
// 	close(server_fd2);
// 	close(kq);
// }


#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <map> // To track client sockets

#define MAX_CLIENTS 5

void runServers(int port1, int port2) {
	int server_fd1, server_fd2;
	sockaddr_in address1, address2;

	// Create the first socket
	server_fd1 = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd1 == -1) {
		perror("Socket creation failed for port 1");
		return;
	}
	int flags = fcntl(server_fd1, F_GETFL, 0);
    if (flags == -1 || fcntl(server_fd1, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("Failed to set non-blocking mode for port 1");
        close(server_fd1);
        return;
    }
	address1.sin_family = AF_INET;
	address1.sin_addr.s_addr = INADDR_ANY;
	address1.sin_port = htons(port1);

	if (bind(server_fd1, (sockaddr*)&address1, sizeof(address1)) < 0) {
		perror("Bind failed for port 1");
		close(server_fd1);
		return;
	}
	if (listen(server_fd1, MAX_CLIENTS) < 0) {
		perror("Listen failed for port 1");
		close(server_fd1);
		return;
	}

	// Create the second socket
	server_fd2 = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd2 == -1) {
		perror("Socket creation failed for port 2");
		close(server_fd1);
		return;
	}
	 flags = fcntl(server_fd2, F_GETFL, 0);
    if (flags == -1 || fcntl(server_fd2, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("Failed to set non-blocking mode for port 2");
        close(server_fd1);
        close(server_fd2);
        return;
    }
	address2.sin_family = AF_INET;
	address2.sin_addr.s_addr = INADDR_ANY;
	address2.sin_port = htons(port2);

	if (bind(server_fd2, (sockaddr*)&address2, sizeof(address2)) < 0) {
		perror("Bind failed for port 2");
		close(server_fd1);
		close(server_fd2);
		return;
	}
	if (listen(server_fd2, MAX_CLIENTS) < 0) {
		perror("Listen failed for port 2");
		close(server_fd1);
		close(server_fd2);
		return;
	}

	std::cout << "Servers running on ports " << port1 << " and " << port2 << "...\n";

	// Create a kqueue instance
	int kq = kqueue();
	if (kq == -1) {
		perror("kqueue creation failed");
		close(server_fd1);
		close(server_fd2);
		return;
	}

	// Register the server sockets with kqueue
	struct kevent ev_set[2];
	EV_SET(&ev_set[0], server_fd1, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
	EV_SET(&ev_set[1], server_fd2, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
	if (kevent(kq, ev_set, 2, nullptr, 0, nullptr) == -1) {
		perror("kevent registration failed");
		close(server_fd1);
		close(server_fd2);
		close(kq);
		return;
	}

	// Event loop
	struct kevent ev_list[10]; // To store triggered events
	std::map<int, std::string> client_buffers; // Track client buffers

	while (true) {
		int nev = kevent(kq, nullptr, 0, ev_list, 10, nullptr);
		if (nev < 0) {
			perror("kevent error");
			break;
		}

		for (int i = 0; i < nev; i++) {
			int fd = ev_list[i].ident; // File descriptor with activity

			if (fd == server_fd1 || fd == server_fd2) {
				// Accept a new client connection
				sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);
				int client_fd = accept(fd, (sockaddr*)&client_addr, &client_len);
				if (client_fd < 0) {
					perror("Accept failed");
				} else {
					std::cout << "Client connected on port " 
					          << (fd == server_fd1 ? port1 : port2) << "\n";
					EV_SET(&ev_set[0], client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
					if (kevent(kq, ev_set, 1, nullptr, 0, nullptr) == -1) {
						perror("Failed to register client socket with kqueue");
						close(client_fd);
					}
				}
			} else {
				// Handle client socket activity
				char buffer[1024];
				ssize_t readbytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
				if (readbytes <= 0) {
					// Client disconnected or error occurred
					if (readbytes < 0)
						perror("Read error");
					else
						std::cout << "Client disconnected\n";
					close(fd);
					client_buffers.erase(fd);
				} else {
					// Append data to the client's buffer
					buffer[readbytes] = '\0';
					for (int i = 0; i < readbytes; i++)
					{
						std::cout << buffer[i];
					}
					
					client_buffers[fd] += buffer;

					// Process complete request (for simplicity, respond immediately)
					std::string response = 
						"HTTP/1.1 200 OK\r\n"
						"Content-Type: text/html\r\n"
						"Content-Length: 45\r\n"
						"\r\n"
						"<html><body><h1>Hello, World!</h1></body></html>";
					send(fd, response.c_str(), response.size(), 0);
					// Uncomment the next line to keep the connection open for more data
					// client_buffers[fd].clear();
					close(fd); // Close the connection after responding
				}
			}
		}
	}

	close(server_fd1);
	close(server_fd2);
	close(kq);
}


int main() {
	int port1 = 8080;
	int port2 = 8081;

	std::string httpRequest = 
    "POST / HTTP/1.1\r\n"
    "Content-Type: multipart/form-data; boundary=--------------------------758740825162635806503399\r\n"
    "User-Agent: PostmanRuntime/7.33.0\r\n"
    "Accept: */*\r\n"
    "Postman-Token: 449c0ab8-3407-4045-b555-15b6e88a1910\r\n"
    "Host: localhost:8080\r\n"
    "Accept-Encoding: gzip, deflate, br\r\n"
    "Connection: keep-alive\r\n"
    "Content-Length: 2111\r\n"
    "\r\n"
    "----------------------------758740825162635806503399\r\n"
    "Content-Disposition: form-data; name=\"file\"; filename=\"client.c\"\r\n"
    "Content-Type: text/x-c\r\n"
    "\r\n"
    "#include <stdio.h>\n"
    "#include <stdlib.h>\n"
    "#include <string.h>\n"
    "#include <unistd.h>\n"
    "#include <arpa/inet.h>\n"
    "#include <.h>\n"
    "\n"
    "#define PORT 8080\n"
    "#define SERVER_IP \"0.0.0.0\"  // Change this to the server's IP if needed\n"
    "#define BUFFER_SIZE 1024\n"
    "\n"
    "int main() {\n"
    "        int sockfd;\n"
    "        struct sockaddr_in server_address;\n"
    "        char buffer[BUFFER_SIZE] = {0};\n"
    "\n"
    "        int fd = open(\"check.c\", O_RDONLY);\n"
    "        printf(\"the fd of the file is :%d\\n\", fd);\n"
    "\n"
    "        // HTTP request with \"Hello, World!\" in the body\n"
    "        const char *http_request = \n"
    "                \"GET / HTTP/1.1\\r\\n\"\n"
    "                \"Host: localhost\\r\\n\";\n";

	std::cout << "the lenght is: " << httpRequest.length() << "\n";
	runServers(port1, port2);

	return 0;
}


