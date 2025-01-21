#include "HttpResponse.hpp"

void HttpResponse::serv_404() const {
    string httpResponse =
    "HTTP/1.1 404 Not Found\r\n"
    "Content-Type: text/html\r\n"
    "Connection: keep-alive\r\n"
    "Content-Length: 389\r\n" // Updated length
    "\r\n"
    "<html>\r\n"
    "<head>\r\n"
    "    <title>404 Not Found</title>\r\n"
    "    <style>\r\n"
    "        body {\r\n"
    "            background-color: black;\r\n"
    "        }\r\n"
    "        h1 {\r\n"
    "            color: white;\r\n"
    "            text-align: center;\r\n"
    "        }\r\n"
    "    </style>\r\n"
    "</head>\r\n"
    "<body>\r\n"
    "    <center><h1>404 Not Found</h1></center>\r\n"
    "    <center><img src=\"https://i.ytimg.com/vi/4m8zqpu7VdM/sddefault.jpg\" alt=\"404 Image\"></center>\r\n"
    "</body>\r\n"
    "</html>\r\n";
    write(request->get_client_socket(), httpResponse.c_str(), httpResponse.size());
    request->set_is_complete(true);
}

void HttpResponse::serv_500() const {
    string htmlBody =
        "<html>\r\n"
        "<head>\r\n"
        "    <title>500 Internal Server Error</title>\r\n"
        "    <style>\r\n"
        "        body {\r\n"
        "            background-color: black;\r\n"
        "        }\r\n"
        "        h1 {\r\n"
        "            color: white;\r\n"
        "            text-align: center;\r\n"
        "        }\r\n"
        "    </style>\r\n"
        "</head>\r\n"
        "<body>\r\n"
        "    <center><h1>500 Internal Server Error</h1></center>\r\n"
        "    <center><img src=\"https://i.ytimg.com/vi/4m8zqpu7VdM/sddefault.jpg\" alt=\"500 Image\"></center>\r\n"
        "</body>\r\n"
        "</html>\r\n";

    // Calculate the content length
    size_t contentLength = htmlBody.size();

    // Construct the full HTTP response
    string httpResponse =
        "HTTP/1.1 500 Internal Server Error\r\n"
        "Content-Type: text/html\r\n"
        "Connection: keep-alive\r\n"
        "Content-Length: " + to_string(contentLength) + "\r\n"
        "\r\n" +
        htmlBody;

    // Send the response
    write(request->get_client_socket(), httpResponse.c_str(), httpResponse.size());
    request->set_is_complete(true);
}
