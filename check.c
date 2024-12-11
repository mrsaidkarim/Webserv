#include <stdio.h>
#include <string.h>

#define RESPONSE_OK "HTTP/1.1 200 OK"

// Function to check if the response is OK
int is_response_ok(const char *response) {
    return strncmp(response, RESPONSE_OK, strlen(RESPONSE_OK)) == 0;
}

int main() {
    // Sample response to test
    const char *http_response = 
        "HTTP/1.1 300 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 45\r\n"
        "\r\n"
        "<html><body><h1>Hello, World!</h1></body></html>";

    // Check if response is OK
    if (is_response_ok(http_response)) {
        printf("Response is OK\n");
    } else {
        printf("Response is NOT OK\n");
    }
    return 0;
}