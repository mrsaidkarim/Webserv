#ifndef CONST_HPP
#define CONST_HPP

#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <sstream> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <dirent.h>
#include <ctime>
#include <sys/event.h>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <sys/time.h>
#include <cstdio> 

extern int kq;
#define vector std::vector
#define string std::string
#define map std::map
#define pair std::pair
#define cout std::cout
#define cerr std::cerr
#define stringStream std::stringstream 
#define endl std::endl
#define setw std::setw
#define right std::right
#define left std::left
#define make_pair std::make_pair
#define fstream std::fstream
#define ios std::ios
#define getline std::getline
#define to_string std::to_string
#define filesystem std::filesystem
#define ostringstream std::ostringstream
#define hex std::hex
#define list std::list
#define streampos std::streampos
#define unordered_map std::unordered_map
#define to_string std::to_string
#define to_integer std::to_integer
#define transform std::transform
#define copy std::copy


// newline characters 
#define CR "\r"
#define LF "\n"
#define CRLF "\r\n"
#define	CRLF_2 "\r\n\r\n"
#define POST_PATH "/Users/zech-chi/goinfre/upload_store/"
#define CGI_PATH "/tmp/"

// Reset Color
#define RESET "\033[0m"

// Text Colors
#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

// Bold Text Color
#define BOLD_BLACK "\033[1;30m"
#define BOLD_RED "\033[1;31m"
#define BOLD_GREEN "\033[1;32m"
#define BOLD_YELLOW "\033[1;33m"
#define BOLD_BLUE "\033[1;34m"
#define BOLD_MAGENTA "\033[1;35m"
#define BOLD_CYAN "\033[1;36m"
#define BOLD_WHITE "\033[1;37m"

// Background Colors
#define BG_BLACK "\033[40m"
#define BG_RED "\033[41m"
#define BG_GREEN "\033[42m"
#define BG_YELLOW "\033[43m"
#define BG_BLUE "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN "\033[46m"
#define BG_WHITE "\033[47m"

#define BUFFER_SIZE (1024 * 5)
#define BUFFER_SIZE2 (1024 * 5)


// PATH TO PAGE
#define FORBIDDEN  "./pages_html/403.html"
#define NOT_FOUND "./pages_html/404.html"
#define NOT_ALLOWED "./pages_html/405.html"
#define DEL_SUCCESS "./delete_successful.html"
#define INTERNAL_SERVER_ERROR "./pages_html/500.html"

#endif