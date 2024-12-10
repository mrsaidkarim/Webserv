#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "../const.hpp"
#include "../Configuration/Server.hpp"
#include "../Request/HttpRequest.hpp"

class HttpResponse {
    private:
        HttpRequest *request;

    public:
        HttpResponse(HttpRequest *);
        ~HttpResponse();

        void get_method();
        void post_method();
        void delete_method();

        void    serv();
        void    serv_404();
        void    serv_redirection();

        
        pair<int, int>    longest_common_location();
        vector<string>    split_url(const string &); // zelabbas radi tmsah mn ba3d

        HttpRequest *get_request() const;

        // get method help functions
        void    get_response_autoindex(string &);
        void    get_response_html(string &);
        void    get_response_txt();
        void    get_response_png();
        void    get_response_mp4();
        void    get_response_mp3();
        void    get_response_pdf();

        void   send_response(string &);
        void   send_chunked_response(string &);
        // post method help functions


        // delete method help functions

};

#endif