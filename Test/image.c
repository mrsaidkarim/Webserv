#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 8000
#define BUFFER_SIZE 1024

// Function to load the image file into memory
int load_image(const char *filename, char **image_data) {
	int fd = open(filename, O_RDONLY);
	if (fd < 0) {
		perror("Failed to open image file");
		return -1;
	}

	// Get the size of the file
	struct stat st;
	if (fstat(fd, &st) != 0) {
		perror("Failed to get file size");
		close(fd);
		return -1;
	}

	int file_size = st.st_size;
	*image_data = (char *)malloc(file_size);
	if (*image_data == NULL) {
		perror("Failed to allocate memory");
		close(fd);
		return -1;
	}

	// Read the file into the allocated memory
	if (read(fd, *image_data, file_size) != file_size) {
		perror("Failed to read image file");
		close(fd);
		free(*image_data);
		return -1;
	}

	close(fd);
	return file_size;
}

int main() {
	int server_fd, new_socket;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	char buffer[BUFFER_SIZE] = {0};

	// Load image data into memory
	char *image_data;
	int image_size = load_image("img.png", &image_data);
	if (image_size < 0) {
		fprintf(stderr, "Failed to load image\n");
		exit(EXIT_FAILURE);
	}

	// HTTP headers for the image response
	char response_headers[BUFFER_SIZE];
	snprintf(response_headers, sizeof(response_headers),
			 "HTTP/1.1 404 KO\r\n"
			 "Content-Type: image/jpeg\r\n"
			 "Content-Length: %d\r\n"
			 "\r\n"
			 "hello master zak",
			 image_size);

	// Step 1: Create socket
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("Socket failed");
		exit(EXIT_FAILURE);
	}

	// Step 2: Bind socket to port
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("Bind failed");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	// Step 3: Start listening for connections
	if (listen(server_fd, 3) < 0) {
		perror("Listen failed");
		close(server_fd); 
		exit(EXIT_FAILURE);
	}

	printf("Server listening on port %d\n", PORT);

	// Step 4: Accept and handle incoming connections
	while (1) {
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
			perror("Accept failed");
			continue;
		}

		// Read client request (not parsing the request here)
		printf("----------------------------------------------\n");
		read(new_socket, buffer, BUFFER_SIZE);
		printf("Received request:\n%s\n", buffer);
		printf("----------------------------------------------\n");
		// Step 5: Send HTTP headers
		write(new_socket, response_headers, strlen(response_headers));

		// Step 6: Send the image data
		write(new_socket, image_data, image_size);

		// Step 7: Close connection
		close(new_socket);
	}

	// Free allocated image data
	free(image_data);

	return 0;
}
