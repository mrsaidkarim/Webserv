#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"  // Change this to the server's IP if needed
#define BUFFER_SIZE 1024

int main() {
	int sockfd;
	struct sockaddr_in server_address;
	char buffer[BUFFER_SIZE] = {0};

	int fd = open("check.c", O_RDONLY);
	printf("the fd of the file is :%d\n", fd);

	// HTTP request with "Hello, World!" in the body
	const char *http_request = 
		"GET / HTTP/1.1\r\n"
		"Host: localhost\n"
		"Content-Length: 13\n"
		"\r\n"
		"Hello, World!";

	// Step 1: Create a socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Socket creation failed");
		return 1;
	}
	printf("the socket fd is :%d\n", sockfd);

	// Step 2: Set up the server address
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);

	// Convert IP address from text to binary form
	if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0) {
		perror("Invalid address/ Address not supported");
		close(sockfd);
		return 1;
	}

	// Step 3: Connect to the server
	if (connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
		perror("Connection failed");
		close(sockfd);
		return 1;
	}
	printf("Connected to server at %s:%d\n", SERVER_IP, PORT);

	// Step 4: Send the HTTP request
	if (send(sockfd, http_request, strlen(http_request), 0) < 0) {
		perror("Failed to send request");
		close(sockfd);
		return 1;
	}
	printf("HTTP request sent:\n%s\n", http_request);

	// Step 5: Read the server's response
	int bytes_received = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
	if (bytes_received < 0) {
		perror("Failed to receive response");
	} else {
		buffer[bytes_received] = '\0'; // Null-terminate the received data
		printf("Server response:\n%s\n", buffer);
	}

	// Step 6: Close the socket
	close(sockfd);
	return 0;
}
/*
sdfjjakldsjfkasdfkasdkfjaskdfas
dfksdfjadlkfjaksdfhasdfas
dfskdfjhjksdfahsd
fsdjfk dshfajls dfkas
dfsjkdfahsjdfhafdadfjk asfhlda
dsfajsdhfk dfha
fsdjfhdksafhdlaf
sdfjskdlhfaj f
asjdkfhaksdfhadf
adfjakdsfhadslf
asdfkjdsjkfkadsf
df afjkhdkasfkajf d
afjhaksfhas
faksfhlakfddsklf
asdfjhkahdfjkadf
adfjakdlhfjakdsf
adfkjahsdfkahflkja
sdfkhasdjkfhadsj
afhjdkhasjkhfa
sdfjkdsjfhdksafd
asfdskfhlsdf
adsfjksdfhadf
adsfkhalsdf
adskfhasdjkfasdfahdjfja
dsfksadfhajdsf
asdfkahsdfjkahsd
fkadhsjf
*/
