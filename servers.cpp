#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define MAX_CLIENTS 5

void runServers(int port1, int port2) {
	int server_fd1, server_fd2;
	sockaddr_in address1{}, address2{};

	// Create the first socket
	server_fd1 = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd1 == -1) {
		perror("Socket creation failed for port 1");
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

	std::cout << "the size total of fds is: " << FD_SETSIZE << "\n";
	std::cout << "Servers running on ports " << port1 << " and " << port2 << "...\n";

	// Use select() to monitor both sockets
	fd_set readfds;
	int max_fd = std::max(server_fd1, server_fd2);

	while (true) {
		FD_ZERO(&readfds);
		FD_SET(server_fd1, &readfds);
		FD_SET(server_fd2, &readfds);

		int activity = select(max_fd + 1, &readfds, nullptr, nullptr, nullptr);

		if (activity < 0) {
			perror("select() error");
			break;
		}

		// Check if there's activity on the first server
		if (FD_ISSET(server_fd1, &readfds)) {
			sockaddr_in client_addr{};
			socklen_t client_len = sizeof(client_addr);
			int client_fd = accept(server_fd1, (sockaddr*)&client_addr, &client_len);
			if (client_fd < 0) {
				perror("Accept failed on port 1");
			} else {
				std::cout << "Client connected on port " << port1 << "\n";
				const char *http_response = 
					"HTTP/1.1 200 OK\r\n"
				"Content-Type: text/html\r\n"
				"Content-Length: 45\r\n"
				"\r\n"
				"<html><body><h1>Hello, World1!</h1></body></html>";
				send(client_fd, http_response, strlen(http_response), 0);
				close(client_fd);
			}
		}

		// Check if there's activity on the second server
		if (FD_ISSET(server_fd2, &readfds)) {
			sockaddr_in client_addr{};
			socklen_t client_len = sizeof(client_addr);
			int client_fd = accept(server_fd2, (sockaddr*)&client_addr, &client_len);
			if (client_fd < 0) {
				perror("Accept failed on port 2");
			} else {
				std::cout << "Client connected on port " << port2 << "\n";
				const char *http_response = 
					"HTTP/1.1 200 OK\r\n"
					"Content-Type: text/html\r\n"
					"Content-Length: 45\r\n"
					"\r\n"
					"<html><body><h1>Hello, World2!</h1></body></html>";
				send(client_fd, http_response, strlen(http_response), 0);
				close(client_fd);
			}
		}
	}

	close(server_fd1);
	close(server_fd2);
}

int main() {
	int port1 = 8080;
	int port2 = 8081;

	runServers(port1, port2);

	return 0;
}
