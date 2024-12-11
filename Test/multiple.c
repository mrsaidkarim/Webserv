#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT1 8080
#define PORT2 9090
#define BUFFER_SIZE 1024

void setup_server_socket(int *server_fd, int port, struct sockaddr_in *address) {
    *server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*server_fd == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(port);

    if (bind(*server_fd, (struct sockaddr *)address, sizeof(*address)) < 0) {
        perror("Bind failed");
        close(*server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(*server_fd, 3) < 0) {
        perror("Listen failed");
        close(*server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d\n", port);
}

int main() {
    int server_fd1, server_fd2, new_socket;
    struct sockaddr_in address1, address2, client_address;
    socklen_t addrlen = sizeof(client_address);
    char buffer[BUFFER_SIZE] = {0};

    // Set up server sockets on two ports
    setup_server_socket(&server_fd1, PORT1, &address1);
    setup_server_socket(&server_fd2, PORT2, &address2);

    // Main loop to handle incoming connections
    while (1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(server_fd1, &readfds);
        FD_SET(server_fd2, &readfds);

        int max_fd = (server_fd1 > server_fd2 ? server_fd1 : server_fd2) + 1;

        // Use select to monitor both sockets
        int activity = select(max_fd, &readfds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("Select error");
            exit(EXIT_FAILURE);
        }

        // Check if there's activity on the first port (8080)
        if (FD_ISSET(server_fd1, &readfds)) {
            new_socket = accept(server_fd1, (struct sockaddr *)&client_address, &addrlen);
            if (new_socket < 0) {
                perror("Accept failed");
                continue;
            }
            printf("Connection accepted on port 8080 from %s:%d\n", 
                   inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            write(new_socket, "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello from 8080!", 56);
            close(new_socket);
        }

        // Check if there's activity on the second port (9090)
        if (FD_ISSET(server_fd2, &readfds)) {
            new_socket = accept(server_fd2, (struct sockaddr *)&client_address, &addrlen);
            if (new_socket < 0) {
                perror("Accept failed");
                continue;
            }
            printf("Connection accepted on port 9090 from %s:%d\n", 
                   inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            write(new_socket, "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello from 9090!", 56);
            close(new_socket);
        }
    }

    close(server_fd1);
    close(server_fd2);

    return 0;
}
