#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 7070
#define BUFFER_SIZE 1024


int main() {
	int server_fd, new_socket;
	struct sockaddr_in address,  client_address;
	int addrlen = sizeof(address);
	int client_addresslen = sizeof(client_address);
	char buffer[BUFFER_SIZE] = {0};
		
	// HTTP response headers and body
	const char *http_response = 
		"HTTP/1.1 500 Not Found\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 45\r\n"
		"\r\n"
		"<html><body><h1>Hello, World!</h1></body></html>";
		
	// Step 1: Create socket
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Step 2: Bind socket to port
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);


	int yes = 1;
	if (setsockopt(server_fd ,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
		perror("setsockopt");
		close(server_fd);
		exit(EXIT_FAILURE);
	}


	printf("the fdsocket is %d\n", server_fd);
	printf("The server address and port are: %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

	printf("the size is :%lu\n", sizeof(address));
	printf("the length is : %d\n", addrlen);
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind failed");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	printf("the fdsocket is %d\n", server_fd);
	printf("The server address and port are: %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

	// int yes = 1;
	// if (setsockopt(server_fd ,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
	// 	perror("setsockopt");
	// 	close(server_fd);
	// 	exit(EXIT_FAILURE);
	// }

	// Step 3: Start listening for connections
	if (listen(server_fd, 3) < 0) {
		perror("listen failed");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	printf("Server listening on port %d\n", PORT);

	// Step 4: Accept and handle incoming connections
	while (1) {
		if ((new_socket = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t*)&client_addresslen)) < 0) {
			perror("accept failed");
			continue;
		}
		printf("the sockerfd is :%d\n", new_socket);
		printf("\nConnection accepted from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));


		// Read client request (not actually parsing, just reading it)
		read(new_socket, buffer, BUFFER_SIZE);
		printf("Received request:\n%s\n", buffer);

		// Step 5: Send HTTP response
		write(new_socket, http_response, strlen(http_response));
		printf("the response is : %s\n", http_response);
		
		// Step 6: Close connection
		close(new_socket);

		printf("----------------------------------------------------------------------------------------");
	}

	return 0;
}


// lose the pesky "Address already in use" error message
// int yes = 1;
// if (setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
// 	perror("setsockopt");