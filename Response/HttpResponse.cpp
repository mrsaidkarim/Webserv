#include "HttpResponse.hpp"

HttpResponse::HttpResponse(HttpRequest *_request) {
    this->request = _request;
}

HttpResponse::~HttpResponse() {

}


void    HttpResponse::serv() {

	// ! here we should check if the request is good or not by see the status_code attribut in the request if is empty the rquest is good! else something is bad
    if (request->get_method() == "GET")
        get_method();
    else if (request->get_method() == "POST")
        post_method();
    else if (request->get_method() == "DELETE")
        delete_method();
}


void HttpResponse::post_method() const {

}

// void HttpResponse::delete_method() const {

// }

HttpRequest *HttpResponse::get_request() const {
    return this->request;
}