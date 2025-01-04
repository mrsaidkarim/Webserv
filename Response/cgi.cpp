#include "HttpResponse.hpp"



char** HttpResponse::header_to_env() const {
    char** env = new char*[request->get_header().size() + 1];
    int i = 0;
    map<string, string>
    for
}