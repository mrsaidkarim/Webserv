# Webserv

Webserv is a lightweight HTTP server designed to demonstrate how sockets and the HTTP protocol work together to enable communication between clients and servers. This project serves as a foundational step towards understanding how web servers operate.


### SOCKETS
sockets are a fundamental component for communication between the server and clients. Sockets allow the server to listen for incoming connections, send and receive data, and manage multiple clients concurrently in a networked environment. A socket is an endpoint for sending or receiving data across a computer network

### Types of Sockets
1. Stream Sockets (TCP Sockets):
    - Used for reliable, connection-oriented communication (TCP).
    - Most web servers use TCP sockets for HTTP communication, as HTTP requires reliable data transmission.
2. Datagram Sockets (UDP Sockets):
    - Used for connectionless communication (UDP), which doesn't guarantee delivery or order of messages.
    - These are less common for web servers because HTTP requires reliability.

### How Sockets Work
1. Creation: A socket is created using the socket() system call.
2. Binding: A server socket binds to an IP address and port to identify the communication endpoint.
3. Listening: A server listens for incoming connection requests.
4. Connecting: A client connects to the server using its IP address and port.
5. Communication: Data is exchanged between the client and the server using send, recv, read, or write.
6. Closure: Once communication is complete, the socket is closed using the close() function.

This diagram explains how sockets work:
https://www.tldraw.com/r/GrwragUxW3rwoaPJdZbk-?d=v-776.-672.2460.2216.page

### HTTP
The Hypertext Transfer Protocol defines a set of rules that clients and servers have to follow in order to communicate with each other for webpages

#### General Formatting of Requests and Responses
There are two main parts of a request / response, the header and the body.
The header contains information on the request / response, describing the context information.
The body contains the content of the request / response, which may be empty.
This string of characters must be present before and after the body (if it exists), which signifies its start and end point.

#### Request
The header of the request starts with a request-line, which specifies (in order) the request method, the request URI and the HTTP version.

``GET /index.html HTTP/1.1``

After the request-line, there may be header fields that describe the context of the request.
The following is an example of a GET request, sent by the browser when a client was trying to access index.html.

```
GET /index.html HTTP/1.1
Host: localhost
Connection: keep-alive
sec-ch-ua: "Chromium";v="116", "Not)A;Brand";v="24", "Opera GX";v="102"
sec-ch-ua-mobile: ?0
sec-ch-ua-platform: "Windows"
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/116.0.0.0 Safari/537.36 OPR/102.0.0.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7
Sec-Fetch-Site: none
Sec-Fetch-Mode: navigate
Sec-Fetch-User: ?1
Sec-Fetch-Dest: document
Accept-Encoding: gzip, deflate, br
Accept-Language: en-US,en;q=0.9
```

This diagram explains how requests work: https://coggle.it/diagram/ZFjiviHgHRe8f6T2/t/request

#### Response
The response is very similar to the request, in which it starts with a response-line, which specifies (in order) the HTTP version, the status code and the status message.

HTTP/1.1 200 OK
Similar to requests, the response might have header fields and also a body.

The following is an example of a response to a request.

HTTP/1.1 200 OK
Connection: keep-alive
Content-Type: text/html
Content-Length: 2462

```
<!DOCTYPE html>
<html>
        <head>
                <style>
                        body {
                        font-family: Arial, Helvetica, sans-serif;
                        background-color: #000000;
                        }
...
...
...
</html>
```