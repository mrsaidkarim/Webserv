#include "HttpResponse.hpp"


HttpResponse::HttpResponse(HttpRequest *_request, WebServ *_webserv) {
    this->request = _request;
    this->webserv = _webserv;
}

HttpResponse::~HttpResponse() {

}


void HttpResponse::check_post_location() {
    index_location = longest_common_location().first;
    cerr << BOLD_RED << index_location << "\n";
    // location not found
    if (index_location == -1) {
        request->set_is_complete_post(true);
        request->set_file_path(NOT_FOUND);
        return;
    }
    // method not allowed
    if (!is_allowed(index_location, "POST")) {
        request->set_is_complete_post(true);
        request->set_file_path(NOT_ALLOWED);
        return;
    }
    // request->get_server().get_locations()[index_location].print_lacation_info();
}

const string& HttpResponse::get_script_path() const{
    vector<string> route = request->get_url();
    // check for root
    request->set_cgi_input_file(request->get_server().get_locations()[index_location].get_root());
    // if not root for this location use global root
    if (request->get_cgi_input_file().empty())
     request->set_cgi_input_file(request->get_server().get_global_root());
    for (int i = 0; i < route.size(); i++) {
        if (i > 0)
            request->set_cgi_input_file( request->get_cgi_input_file() + "/");
        request->set_cgi_input_file( request->get_cgi_input_file() + route[i]);
    }
    return request->get_cgi_input_file();
}


void    HttpResponse::serv() {

    // check if it is a cgi
	// ! here we should check if the request is good or not by see the status_code attribut in the request if is empty the rquest is good! else something is bad
    if (request->get_header().find("cookie") != request->get_header().end() && request->get_url().size() == 1
        && request->get_url()[0] == "cookie") {
        int pos = request->get_header().find("cookie")->second.find("session_id=");
        if (pos != string::npos) {
            string session_path = request->get_header().find("cookie")->second.substr(pos + 11);
            if (is_a_file(SESSION_MANAGEMENT + session_path)) {
                request->set_file_path(SESSION_MANAGEMENT + session_path);
                request->set_is_chunked(true);
            }
            cout << BG_GREEN << "("  << session_path << ")" << "\n";
        }
    }
    if (request->get_method() == "GET")
        get_method();
    else if (request->get_method() == "POST") {
        check_post_location();
        post_method();
    }
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
