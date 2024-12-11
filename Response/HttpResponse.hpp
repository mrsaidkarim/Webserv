#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "../const.hpp"
#include "../Configuration/Server.hpp"
#include "../Request/HttpRequest.hpp"

class HttpResponse {
    private:
        HttpRequest request;
        vector<Server> servers;

    public:
        HttpResponse();
        HttpResponse(const HttpRequest& request, const vector<Server> &servers);
        ~HttpResponse();

        void get_method() const;
        void post_method() const;
        void delete_method() const;

        void    serv();


        // get method help functions
        void    get_response_html();
        void    get_response_txt();
        void    get_response_png();
        void    get_response_mp4();
        void    get_response_mp3();
        void    get_response_pdf();

        // post method help functions


        // delete method help functions

};

#endif