/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skarim <skarim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 16:00:30 by zelabbas          #+#    #+#             */
/*   Updated: 2025/01/19 12:03:18 by skarim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_H
# define HTTPREQUEST_H

#include "../const.hpp"
#include "../Configuration/Server.hpp"

class HttpRequest
{
	private:
		string 				method; // get post delete
		vector<string> 		url;
		string 				version; // http version
		string 				body; // body of the request
		string				query; 
		string				fragment;
		string 				statusCode;
		map<string, string>	header;
		map<string, char>	encoding_symbols;
		fstream				*file_stream; // file stream
		bool				is_binary_post;// for post binary case
		string			    file_path; // request file path 
		streampos			file_offset; // file offset
		bool 				is_chunked; // is chunked response
		size_t				chunked_post_offset; // position of serach for the chunk_size pattern;
		bool 				is_complete; // if offset is at the end of the file
		bool				is_complete_post;
		bool				is_cgi;
		bool				is_unlink_file_path;
		string				cgi_path_post;
		Server				server; // the server will serve the request
		int 				client_socket; // file descriptor of communication socket
		size_t				read_content_length;
		string				boundary_key; // for post method
		string 				boundary_key_begin;
		string				boundary_key_end;
		bool    			cgi_in_process;
		bool				was_cgi;
		string				cgi_output_file;
		string				session_id;
		string 				cgi_input_file;
		int					cookie;
		string              path_info;
		long			content_length;
		string				request;
		bool				is_cgi_complete;

	protected:
		vector<string>	split(const string& str, char delimiter);
		string			trim_string(const string& str);
		void			upper_to_lower(string& str);
		bool			check_url_characters(const string& _url);
		bool			is_valid_characters(const string& str);
		bool			is_valid_value(const string& _value);
		bool			is_start_with_space(const string& str);
		bool			is_valid_header_request(const string& _header);
		bool			check_header_elements();
	public:
		HttpRequest(const string& _request);
		~HttpRequest();

		void http_request_init();

		// SETTERS:
		void set_encoding_symbols();
		void set_status_code(const string& _status);
		void set_fragment(const string& _fragment);
		void set_query(const string& _query);
		void set_body(const string& _body);
		bool set_method(const string& _method);
		bool set_url(const string& _url);
		bool set_version(const string& _version);
		bool set_map(const string& _key, const string& _value);
		void set_client_socket(int _client_socket);
		void set_server(const Server& _server);
		void set_is_complete(bool _is_complete);
		void set_is_chunked(bool _is_chunked);
		void set_file_path(const string& _file_path);
		void set_file_stream(fstream* _file_stream);
		void set_file_offset(streampos _file_offset);
		bool set_boundary_key(void);
		void set_is_cgi(bool is_cgi);
		void set_is_unlink_file_path(bool is_unlink);
		void set_cgi_in_process(bool _cgi_in_process);
		void set_is_complete_post(bool _is_complete_post);
		void set_was_cgi(bool _was_cgi);
		void set_cgi_file_path(const string& _file_path);
		void set_session_id(const string& _session_id);
		void set_cgi_input_file(const string& _cgi_file_input);
		void set_cgi_path_post(const string& _cgi_path_post);
		void set_cookie(int _cookie);
		void set_is_binary_post(bool is_binary_post);
		void set_content_length(long content_length);
		void set_path_info(string& _url);
		void set_is_cgi_complete(bool _is_cgi_complete);
		void set_chunked_post_offset(size_t _chunked_post_offset);

		
		// GETTERS:
		const string& get_status_code(void) const;
		const string& get_method(void) const;
		const vector<string>& get_url(void) const;
		const string& get_version(void) const;
		const string& get_body(void) const;
		const string& get_query(void) const;
		const string& get_fragment(void) const;
		const map<string, string>& get_header(void) const;
		int get_client_socket(void) const;
		const Server& get_server(void) const;
		bool get_is_complete(void) const;
		bool get_is_chunked(void) const;
		const string& get_file_path(void) const;
		fstream* get_file_stream(void) const;
		streampos get_file_offset(void) const;
		const string& get_boundary_key_begin(void) const;
		const string& get_boundary_key_end(void) const;
		bool get_is_cgi(void) const;
		bool get_is_unlink_file_path(void) const;
		const string& get_cgi_path_post(void) const;
		bool get_cgi_in_process(void) const;
		bool get_is_complete_post(void) const;
		bool get_was_cgi(void) const;
		const string& get_session_id(void) const;
		const string& get_cgi_input_file(void) const;
		int get_cookie(void) const;
		const string& get_path_info(void) const;
		bool get_is_cgi_complete(void) const;
		size_t get_chunked_post_offset(void) const;

		// for post method
		void	add_to_body(const string &, int );
		bool get_is_binary_post(void) const;
		long get_content_length(void) const;
		void check_chunked();
		
		void  display_request();
		string get_status_line();
};

#endif
