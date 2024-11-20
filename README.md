# Webserv
This is when you finally understand why a URL starts with HTTP

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