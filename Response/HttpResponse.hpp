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


        // delete method help functions
		bool	is_allowed(int index_location) const;
		void	handle_file(const string& path) const;
		bool	does_not_exist(const string& path) const;
		bool	is_a_file(const string& path) const;
		bool	delete_file(const string& file_path) const;
};

#endif