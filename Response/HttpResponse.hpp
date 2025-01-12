#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "../const.hpp"
#include "../Configuration/Server.hpp"
#include "../Request/HttpRequest.hpp"

class HttpResponse {
    private:
        HttpRequest *request;
        vector<Server> servers;

    public:
        HttpResponse(HttpRequest *);
        ~HttpResponse();

        void get_method() const;
        void post_method() const;
        void delete_method() const;

        void    serv();



        void serv_404() const;

        string get_content_type(const string &) const;
        void send_response() const;
        void serv_redirection() const;
        void serv_autoindex(const string&) const;

        // get method help functions
        pair<int, int> longest_common_location() const;
        HttpRequest *get_request() const;
        // post method help functions
        void binary_post_case();
        // delete method help functions

};

#endif