// #include <arpa/inet.h>
// #include <netdb.h>
// #include <netinet/in.h>
// #include <stdio.h>
// #include <sys/socket.h>
// #include <sys/types.h>


// int main()
// {
//     struct addrinfo *pai = NULL;
//     int err = getaddrinfo("localhost", NULL, NULL, &pai);
//     if (err != 0) {
//         fprintf(stderr, "error in getaddrinfo: %s\n", gai_strerror(err));
//         return -1;
//     }
//     if (pai->ai_family == AF_INET) {
//         struct sockaddr_in *psai = (struct sockaddr_in*)pai->ai_addr;
//         char ip[INET_ADDRSTRLEN];
//         if (inet_ntop(pai->ai_family, &(psai->sin_addr), ip, INET_ADDRSTRLEN) != NULL) {
//             printf("IP: %s\n", ip);
// 			printf("the port is : %d\n", ntohs(psai->sin_port));
//         }
//     } else if (pai->ai_family == AF_INET6) {
//         struct sockaddr_in6 *psai = (struct sockaddr_in6*)pai->ai_addr;
//         char ip[INET6_ADDRSTRLEN];
//         if (inet_ntop(pai->ai_family, &(psai->sin6_addr), ip, INET6_ADDRSTRLEN) != NULL) {
//             printf("IP: %s\n", ip);
//         }
//     } else {
//         printf("Don't know how to convert family %d addresses\n", pai->ai_family);
//     }
//     freeaddrinfo(pai);
//     printf("\nDone.\n\n");
//     return 0;
// }

loclahost 
any 

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

int main() {
    struct addrinfo *pai = NULL, *p;
    int err = getaddrinfo("googl.com", NULL, NULL, &pai);
    if (err != 0) {
        fprintf(stderr, "error in getaddrinfo: %s\n", gai_strerror(err));
        return -1;
    }

    printf("Resolved addresses for 'localhost':\n");
    for (p = pai; p != NULL; p = p->ai_next) {
        char ip[INET6_ADDRSTRLEN] = {0};
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *psai = (struct sockaddr_in *)p->ai_addr;
            if (inet_ntop(p->ai_family, &(psai->sin_addr), ip, sizeof(ip)) != NULL) {
                printf("IPv4: %s, Port: %d\n", ip, ntohs(psai->sin_port));
            }
        } else if (p->ai_family == AF_INET6) { // IPv6
            struct sockaddr_in6 *psai = (struct sockaddr_in6 *)p->ai_addr;
            if (inet_ntop(p->ai_family, &(psai->sin6_addr), ip, sizeof(ip)) != NULL) {
                printf("IPv6: %s\n", ip);
            }
        } else {
            printf("Unknown address family: %d\n", p->ai_family);
        }
    }

    freeaddrinfo(pai);
    return 0;
}



// #include <arpa/inet.h>
// #include <netdb.h>
// #include <netinet/in.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/socket.h>
// #include <sys/types.h>

// int main() {
//     struct addrinfo hints, *pai = NULL;

//     // Initialize hints
//     memset(&hints, 0, sizeof(hints));
//     hints.ai_family = AF_INET;       // Only IPv4 addresses
//     hints.ai_socktype = SOCK_STREAM; // TCP sockets
//     hints.ai_flags = AI_CANONNAME;   // Request canonical name

//     // Resolve address
//     int err = getaddrinfo("www.example.com", "http", &hints, &pai);
//     if (err != 0) {
//         fprintf(stderr, "error in getaddrinfo: %s\n", gai_strerror(err));
//         return -1;
//     }

//     // Iterate through results
//     struct addrinfo *p = pai;
//     while (p) {
//         if (p->ai_family == AF_INET) {
//             struct sockaddr_in *psai = (struct sockaddr_in *)p->ai_addr;
//             char ip[INET_ADDRSTRLEN];
//             inet_ntop(p->ai_family, &(psai->sin_addr), ip, INET_ADDRSTRLEN);
//             printf("IP: %s, Port: %d\n", ip, ntohs(psai->sin_port));
//         }
//         p = p->ai_next;
//     }

//     freeaddrinfo(pai);
//     return 0;
// }


