#include "HttpResponse.hpp"

char** HttpResponse::header_to_env() const{
    char** env = new char*[request->get_header().size() + 1];
    int i = 0;
    map<string, string>::const_iterator it = request->get_header().begin();
    while (it != request->get_header().end()) {
        string key = it->first;
        string value = it->second;
        transform(key.begin(), key.end(), key.begin(), toupper);
        replace(key.begin(), key.end(), '-', '_');
        it++;
        i++;
    }
}