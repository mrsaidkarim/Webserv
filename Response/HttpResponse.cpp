#include "HttpResponse.hpp"


HttpResponse::HttpResponse(const HttpRequest& request, const vector<Server> &servers) {
    this->request = request;
    this->servers = servers;
}


HttpResponse::~HttpResponse() {

}


void    HttpResponse::serv() {
    if (request.get_method() == "GET")
        get_method();
    else if (request.get_method() == "POST")
        post_method();
    else if (request.get_method() == "DELETE")
        delete_method();
}


void HttpResponse::get_method() const {
    
}

void HttpResponse::post_method() const {

}

void HttpResponse::delete_method() const {

}