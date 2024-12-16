#include "HttpResponse.hpp"

HttpResponse::HttpResponse(HttpRequest *_request) {
    this->request = _request;
}

HttpResponse::~HttpResponse() {

}


void    HttpResponse::serv() {
    if (request->get_method() == "GET")
        get_method();
    else if (request->get_method() == "POST")
        post_method();
    else if (request->get_method() == "DELETE")
        delete_method();
}


void HttpResponse::post_method() const {
    
}


void HttpResponse::delete_method() const {

}

HttpRequest *HttpResponse::get_request() const {
    return this->request;
}