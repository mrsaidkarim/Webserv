#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
	int server_fd, new_socket;
	struct sockaddr_in address, client_address;
	int addrlen = sizeof(address);
	char buffer[BUFFER_SIZE] = {0};
	
	// HTTP response for GET request
	const char *http_response_get = 
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 45\r\n"
		"\r\n"
		"<html><body><h1>Hello, World!</h1></body></html>";
		
	// HTTP response for POST request
	const char *http_response_post = 
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/plain\r\n"
		"Content-Length: 19\r\n"
		"\r\n"
		"POST received successfully";

	// Step 1: Create socket
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Step 2: Bind socket to port
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	printf("The server address is: %s\n", inet_ntoa(address.sin_addr));

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind failed");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	// Step 3: Start listening for connections
	if (listen(server_fd, 3) < 0) {
		perror("listen failed");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	printf("Server listening on port %d\n", PORT);

	// Step 4: Accept and handle incoming connections
	while (1) {
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
			perror("accept failed");
			continue;
		}
		printf("Connection accepted from %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

		// Read client request
		read(new_socket, buffer, BUFFER_SIZE);
		printf("Received request:\n%s\n", buffer);

		// Check if the request is a GET or POST method
		if (strncmp(buffer, "GET", 3) == 0) {
			// Handle GET request
			write(new_socket, http_response_get, strlen(http_response_get));
			printf("Sent GET response:\n%s\n", http_response_get);
		} else if (strncmp(buffer, "POST", 4) == 0) {
			// Handle POST request
			// Find the body of the POST request
			char *body = strstr(buffer, "\r\n\r\n");
			if (body) {
				body += 4;  // Skip the "\r\n\r\n" to get to the start of the body
				printf("Received POST body:\n%s\n", body);
			}

			// Send POST response
			write(new_socket, http_response_post, strlen(http_response_post));
			printf("Sent POST response:\n%s\n", http_response_post);
		} else {
			// Handle other HTTP methods if needed
			printf("Unhandled HTTP method\n");
		}

		// Step 6: Close connection
		close(new_socket);
		memset(buffer, 0, BUFFER_SIZE); // Clear buffer for the next request
	}

	return 0;
}
