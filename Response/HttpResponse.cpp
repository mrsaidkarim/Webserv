#include "HttpResponse.hpp"


HttpResponse::HttpResponse(HttpRequest *_request, WebServ *_webserv) {
    this->request = _request;
    this->webserv = _webserv;
}

HttpResponse::~HttpResponse() {

}

void    HttpResponse::serv() {

    // check if it is a cgi
	// ! here we should check if the request is good or not by see the status_code attribut in the request if is empty the rquest is good! else something is bad
    if (request->get_method() == "GET")
        get_method();
    else if (request->get_method() == "POST")
        post_method();
    else if (request->get_method() == "DELETE")
        delete_method();
}


HttpRequest *HttpResponse::get_request() const {
    return this->request;
}

string HttpResponse::generate_file_name() const {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    // Extract seconds and microseconds
    time_t rawTime = tv.tv_sec;
    int microseconds = tv.tv_usec;
    
    // Convert to local time
    struct tm *timeInfo = localtime(&rawTime);

    // Create the formatted string
    ostringstream oss;
    oss << CGI_PATH
        << (timeInfo->tm_year + 1900) << "_"       // Full Year (e.g., 2025)
        << (timeInfo->tm_mon + 1) << "_"           // Month
        << timeInfo->tm_mday << "_"               // Day
        << timeInfo->tm_hour << "_"               // Hours
        << timeInfo->tm_min << "_"                // Minutes
        << timeInfo->tm_sec << "_"                // Seconds
        << microseconds << ".html";               // Microseconds

    return oss.str();
}