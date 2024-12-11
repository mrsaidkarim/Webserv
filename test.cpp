#include <iostream>
#include <cstring>

int main() {


	std::string http_request = 
	"GET / HTTP/1.1\r\n"
	"Host: localhost\r\n"
	"Content-Length: 13\r\n"
	"tkharbir9a: sfdhaf\r\n"
	"hello man:sdjanfd\r\n"
	"\r\n"
	"Hello, World!";

	std::string notFoundResponse = 
    "<html>\r\n"
    "<head><title>404 Not Found</title></head>\r\n"
    "<body>\r\n"
    "<center><h1>404 Not Found</h1></center>\r\n"
    "<hr><center>nginx/1.27.2</center>\r\n"
    "</body>\r\n"
    "</html>\r\n"
    "<!-- a padding to disable MSIE and Chrome friendly error page -->\r\n"
    "<!-- a padding to disable MSIE and Chrome friendly error page -->\r\n"
    "<!-- a padding to disable MSIE and Chrome friendly error page -->\r\n"
    "<!-- a padding to disable MSIE and Chrome friendly error page -->\r\n"
    "<!-- a padding to disable MSIE and Chrome friendly error page -->\r\n"
    "<!-- a padding to disable MSIE and Chrome friendly error page -->\r\n";

	std::cout << "the length is :" << notFoundResponse.length() << "\n";
	exit(0);

	std::string test = "<HTML>\r\n<HEAD><TITLE>Hello World</Title></HEAD>\r\n<BODY>Hello World</BODY>\r\n</HTML>";
	for (int i = 0; i < 83; i++)
	{
		std::cout << test[i];
	}
	
	std::cout << "\nthe length is: " << test.length() << "\n";

	int pos = http_request.find("\r\n\r\n");
	std::cout << pos << "\n";
	std::string body = http_request.substr(pos + 4); 
	std::cout << http_request << "\n";

	std::cout << "the string is : " + body << "\n";  
	return (0);
}

// fcntl(socket, F_SETFL, O_NONBLOCK)