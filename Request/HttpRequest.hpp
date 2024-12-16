/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelabbas <zelabbas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 16:00:30 by zelabbas          #+#    #+#             */
/*   Updated: 2024/12/02 20:40:42 by zelabbas         ###   ########.fr       */
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
		string			    file_path; // request file path 
		streampos			file_offset; // file offset
		bool 				is_chunked; // is chunked response
		bool 				is_complete; // if offset is at the end of the file
		Server				server; // the server will serve the request
		int 				client_socket; // file descriptor of communication socket
	
	protected:
		vector<string>	split(const string& str, char delimiter);
		string			trim_string(const string& str);
		bool			check_url_characters(const string& _url);
		bool			is_valid_characters(const string& str);
		bool			is_valid_value(const string& _value);
		bool			is_start_with_space(const string& str);
		bool			is_valid_header_request(const string& _header);
		bool			check_header_elements();
	public:
		HttpRequest(const string& _request);
		~HttpRequest();

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

		// for post method

		void	add_to_body(const string &, int );


		void  display_request(); // TO REMOVE
};

#endif