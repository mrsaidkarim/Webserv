/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelabbas <zelabbas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 12:06:33 by skarim            #+#    #+#             */
/*   Updated: 2024/12/11 14:29:53 by zelabbas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "const.hpp"
#include "Configuration/Location.hpp"
#include "Configuration/Server.hpp"
#include "Request/HttpRequest.hpp"

const string http_response =
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: text/html\r\n"
	"Content-Length: 45\r\n"
	"\r\n"
	"<html><body><h1>hello world!</h1></body></html>";

const string badRequest =
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

int main(int ac, char **av)
{
	signal(SIGPIPE, SIG_IGN);
	vector<int> ports = {8080};
	vector<int> sockets;

	for (int port : ports) {
		int sock = create_and_bind_socket(port);
		sockets.push_back(sock);
		cout << "Server listening on port: " << port << "\n";
	}
	fd_set readfds;
	char buffer[BUFFER_SIZE] = {0};
	string line;

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

				cout << "Connection accepted from: " 
						  << inet_ntoa(client_address.sin_addr) << ":" 
						  << ntohs(client_address.sin_port) << "\n";
				ssize_t bytes_read  = 1;

				string http_request;
				while (bytes_read)
				{
					bytes_read = read(new_socket, buffer, BUFFER_SIZE);
					// for (int i = 0; i < bytes_read; i++)
					// {
					// 	outfile << buffer[i];
					// }
					
					
					if (bytes_read < 0) {
						// cout << RED << "error in reading\n" << RESET;
						break;
					}
					// buffer[bytes_read] = '\0';
					if (bytes_read >= 0) {
						for (int i = 0;i < bytes_read; i++)
						{
							http_request += buffer[i];
							// cout << buffer[i];
						}
						

						// cout << "Received request: " << request << "\n";
					// getFirstline(request, line);
					// std::cout << "First line of the request: " << line << "\n";

					// if (check_Line(line))
					// else
					}
					// write(new_socket, badRequest.c_str(), badRequest.length());
					memset(buffer, 0, BUFFER_SIZE);
					// sleep(2);
					write(new_socket, http_response.c_str(), http_response.length());
				}
				cout << BOLD_GREEN << "------> "<< http_request << RESET <<  "\n";
				HttpRequest test(http_request);
				if (test.get_status_code().empty())
					test.display_request();
				else
					cout << RED << "error: " << test.get_status_code() << "\n" << RESET;
				close(new_socket);
			}
		}
	}

    // vector<string> route = {"home", "repo1", "index.html"};
    // vector<string> indexes = {"index.html", "home.html"};
    // bool auto_index = true;
    // string root = "/var/www/example";
    // map<string, bool> methods = {
    //     {"GET", true},
    //     {"POST", true},
    //     {"DELETE", false}
    // };
    // pair<string, string> redirections = {"301", "www.youtube.com"};

    // // Create a Location object
    // Location loc1(route, indexes, auto_index, root, methods, redirections);
    // Location loc2(
    //     {"home", "repo2", "index.html"},
    //     {"home.html"},
    //     false,
    //     "",
    //     {{"GET", true},
    //     {"POST", false},
    //     {"DELETE", false}},
    //     {}
    // );
    // Location loc3(
    //     {"home", "repo3", "index.html"},
    //     {},
    //     false,
    //     "",
    //     {{"GET", true},
    //     {"POST", false},
    //     {"DELETE", false}},
    //     {"301", "www.youtube.com"}
    // );
    // Location.print_lacation_info();

    // vector<int> ports = {8080, 443};
    // vector<string> server_names = {"zechi.com", "www.nigro.com"};
    // long long client_max_body_size = 10485760; // 10 MB
    // vector<Location> locations = {loc1, loc2, loc3};
    // string global_root = "/globalroot";
    // pair<string, string> redirection = {"301", "www.ingtagram.com"};
    // vector<string> indexes2 = {"indexserver.html", "homeserver.html"};
    // bool autoindex2 = true;
    // vector<string> error_pages = {"404.html", "500.html", "403.html"};
    
    // Server server(
    //     ports,
    //     server_names,
    //     client_max_body_size,
    //     locations,
    //     global_root,
    //     redirection,
    //     indexes2,
    //     autoindex2,
    //     error_pages
    // );

    // server.print_server_info();
	// const char *http_request = 
	// "POST /index%7B HTTP/1.1\r\n"
	// "Host: localhost\r\n"
	// "Content-Length: 13 \r\n"
	// "transfer: code\r\n"
	// "Content-Type: multipart/form-data; boundary=--------------------------063651767448020375594229\r\n"
	// "test: HTTPHELLO\r\n"
	// "\r\n"
	// "----------------------------063651767448020375594229\r\n"
	// "Content-Disposition: form-data; name=\"file\"; filename=\"input.csv\"\r\n"
	// "Content-Type: text/csv\r\n"
	// "\r\n"
	// "date | value\n"
	// "2005-02-02 | 0.1\n"
	// "2011-09-14 | 390.57\n"
	// "2010-11-03 | 517.239\n"
	// "2020-06-26 | 998.808\n"
	// "2012-07-09 | 1242.2\n"
	// "2019-07-11 | 700.492\n"
	// "2019-06-25 | 416.636\n"
	// "2017-05-22 | 507.753\n"
	// "2012-12-23 | 1146.69\n"
	// "2013-12-02 | 1293.19\n"
	// "2016-12-12 | 1602.81\n"
	// "2017-04-22 | 737.216\n"
	// "2014-11-26 | 782.601\n"
	// "2015-07-13 | 583.763\n"
	// "2013-04-12 | 353.688\n"
	// "2009-11-06 | 1018.34\n"
	// "2010-11-19 | 396.835\n"
	// "2020-03-29 | 1558.83\n"
	// "2014-01-26 | 1800.22\n"
	// "2016-07-30 | 515.044\n"
	// "2014-03-16 | 632.64"
	// "----------------------------063651767448020375594229--\n";
	// const string str_hex = "3a";
	// HttpRequest test(http_request);
	
	// size_t value;
	// try
	// {
	// 	value = test.hex_to_dic(str_hex);
	// 	cout << BOLD_RED << "the value" << value; 
	// }
	// catch(const std::exception& e)
	// {
	// 	cout << BOLD_RED << e.what() << '\n' << RESET;
	// }

	// int i = -1;
	// while (++i < 2000)
	// {
	// 	test.handle_normal_body(test.get_body());
	// }
	// for (int sock : sockets) {
	// 	close(sock);
	// }
}