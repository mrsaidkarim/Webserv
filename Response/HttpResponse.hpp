#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "../const.hpp"
#include "../Configuration/Server.hpp"
#include "../Request/HttpRequest.hpp"
#include <sys/_types/_pid_t.h>
#include "../WebServ.hpp"

class WebServ;

class HttpResponse {
    private:
        HttpRequest *request;
        vector<Server> servers;
        int index_location;
        WebServ *webserv;

    public:
        HttpResponse(HttpRequest *_request, WebServ *_webserv);
        ~HttpResponse();

        void get_method();
        void post_method();
        void delete_method() const;

        void    serv();



        void serv_404() const;

        string get_content_type(const string &) const;
        void send_response() const;
        void serv_redirection(string redirect_code, string redirect_url) const;
        void serv_autoindex(const string&) const;

        // get method help functions
        pair<int, int> longest_common_location() const;
        HttpRequest *get_request() const;
        // post method help functions
        fstream *binary_post_case();
        bool normalize_chunked_data(string &s);
        // delete method help functions
		bool	is_allowed(int index_location) const;
        bool    is_allowed(int index_location, string method) const;
		void	handle_file(const string& path) const;
		bool	does_not_exist(const string& path) const;
		bool	is_a_file(const string& path) const;
		bool	delete_file(const string& file_path) const;

        // cgi;
        char**  header_to_env() const;
        bool    is_cgi() const;
        void    cgi() const;
        void    print_env(char **env) const;

        string generate_file_name() const;
        void    handle_timeout(pid_t pid, const string& file_path) const;

        void    set_cgi_in_process(bool);
        void    check_post_location();
        const string&  get_script_path() const;


        // cookies 
        void handle_cookie1();
        void handle_cookie2();
};

#endif