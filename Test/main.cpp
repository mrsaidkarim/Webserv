#include <unistd.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <iostream>
#include <vector>
#include <string>

#define RESET "\033[0m"
#define RED "\033[31m"

std::string& getFirstline(std::string& Request, std::string& line) {
	int index = Request.find("\r\n");
	if (index == std::string::npos)
		line = "bad";
	else
		line = Request.substr(0, index);
	return line;
}

bool check_Line(std::string& line) {
	if (!line.compare("bad") || line[0] == ' ') {
		std::cout << "here 301\n";
		return false;
	}
	return true;
}

#define BUFFER_SIZE 4000

const std::string http_response =
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: text/html\r\n"
	"Content-Length: 45\r\n"
	"\r\n"
	"<html><body><h1>hello world!</h1></body></html>";

const std::string badRequest =
	"HTTP/1.1 400 Bad Request\r\n"
	"Content-Type: text/html\r\n"
	"Content-Length: 176\r\n"
	"\r\n"
	"<html>"
	"<body>"
	"<h1>Bad Request</h1>"
	"<img src=\"https://lareleve.ma/wp-content/uploads/2022/03/Untitled_41_303721366.jpg\" alt=\"Sample Image\" width=\"300\" height=\"200\">"
	"</body>"
	"</html>";

int create_and_bind_socket(int port) {
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	int yes = 1;
	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
		perror("setsockopt");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in address{};
	address.sin_family = AF_INET;
	// address.sin_addr.s_addr = INADDR_ANY;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(port);

	if (bind(sock_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
		perror("bind failed");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	if (listen(sock_fd, 3) < 0) {
		perror("listen failed");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	return sock_fd;
}

int main() {
	signal(SIGPIPE, SIG_IGN);
	std::vector<int> ports = {8080};
	std::vector<int> sockets;

	for (int port : ports) {
		int sock = create_and_bind_socket(port);
		sockets.push_back(sock);
		std::cout << "Server listening on port: " << port << "\n";
	}

	fd_set readfds;
	char buffer[BUFFER_SIZE] = {0};
	std::string line;

	while (true) {
		FD_ZERO(&readfds);
		int max_fd = 0;

		for (int sock : sockets) {
			FD_SET(sock, &readfds);
			if (sock > max_fd)
				max_fd = sock;
		}

		int activity = select(max_fd + 1, &readfds, nullptr, nullptr, nullptr);
		if (activity < 0 && errno != EINTR) {
			perror("select error");
			break;
		}

		for (int sock : sockets) {
			if (FD_ISSET(sock, &readfds)) {
				struct sockaddr_in client_address;
				socklen_t client_addresslen = sizeof(client_address);
				int new_socket = accept(sock, (struct sockaddr*)&client_address, &client_addresslen);
				if (new_socket < 0) {
					perror("accept failed");
					continue;
				}

				std::cout << "Connection accepted from: " 
						  << inet_ntoa(client_address.sin_addr) << ":" 
						  << ntohs(client_address.sin_port) << "\n";
				ssize_t bytes_read  = 1;
				while (bytes_read)
				{
					bytes_read = read(new_socket, buffer, BUFFER_SIZE - 1);
					if (bytes_read < 0) {
						std::cout << RED << "error in reading\n" << RESET;
						break;
					}
					// buffer[bytes_read] = '\0';
					if (bytes_read > 0) {
					std::string request(buffer, bytes_read);
					std::cout << "Received request: " << request << "\n";
					// getFirstline(request, line);
					// std::cout << "First line of the request: " << line << "\n";

					// if (check_Line(line))
					write(new_socket, http_response.c_str(), http_response.length());
					// else
					// 	write(new_socket, badRequest.c_str(), badRequest.length());
					}
				}
				close(new_socket);
			}
		}
	}

	for (int sock : sockets) {
		close(sock);
	}

	return 0;
}
