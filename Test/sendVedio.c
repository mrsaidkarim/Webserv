#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 9696
#define BUFFER_SIZE 1024

// Function to load the video file into memory
int load_video(const char *filename, char **video_data) {
	int fd = open(filename, O_RDONLY);
	if (fd < 0) {
		perror("Failed to open video file");
		return -1;
	}

	// Get the size of the file
	struct stat st;
	// stat(filename, &st);

	if (stat(filename, &st) != 0) {
		perror("Failed to get file size");
		close(fd);
		return -1;
	}
	printf("the size is: %lld\n", st.st_size);

	int file_size = st.st_size;
	*video_data = (char *)malloc(file_size);
	if (*video_data == NULL) {
		perror("Failed to allocate memory");
		close(fd);
		return -1;
	}

	printf("the size of the file is : %d\n", file_size);
	// Read the file into the allocated memory
	if (read(fd, *video_data, file_size) != file_size) {
		perror("Failed to read video file");
		close(fd);
		free(*video_data);
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

	// Load video data into memory
	char *video_data;
	int video_size = load_video("video.mp4", &video_data);
	if (video_size < 0) {
		fprintf(stderr, "Failed to load video\n");
		exit(EXIT_FAILURE);
	}

	// HTTP headers for the video response
	char response_headers[BUFFER_SIZE];
	snprintf(response_headers, sizeof(response_headers),
			 "HTTP/1.1 200 OK\r\n"
			 "Content-Type: video/mp4\r\n"
			 "Content-Length: %d\r\n"
			 "\r\n",
			 video_size);

	printf("the response is :%s\n", response_headers);

	// Step 1: Create socket
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
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
		read(new_socket, buffer, BUFFER_SIZE);
		printf("Received request:\n%s\n", buffer);

		// Step 5: Send HTTP headers
		write(new_socket, response_headers, strlen(response_headers));

		// Step 6: Send the video data
		write(new_socket, video_data, video_size);

		// Step 7: Close connection
		close(new_socket);
	}

	// Free allocated video data
	free(video_data);

	return 0;
}
