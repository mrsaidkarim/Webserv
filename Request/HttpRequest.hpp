/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelabbas <zelabbas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 16:00:30 by zelabbas          #+#    #+#             */
/*   Updated: 2024/12/01 12:26:18 by zelabbas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_H
# define HTTPREQUEST_H

#include "../const.hpp"
#include "../Configuration/Server.hpp"

class HttpRequest
{
	private:
		string 				method;
		string 				url;
		string 				version;
		string 				body;
		string 				statusCode;
		int                 serv_socket_communication_fd;
		int 			    file_fd;
		string			    file_path;
		streampos 			file_offset;
		map<string, string> header;
		Server				server;
		bool				is_complete;
		bool 				is_chunked;
	
	protected:
		vector<string>	split(const string& str, char delimiter);
		string			trim_string(const string& str);
		bool			is_valid_characters(const string& str);
		bool			is_start_with_space(const string& str);
		bool			is_valid_header_request(const string& _header);
	public:
		HttpRequest(const string& _request, const int _serv_socket_communication_fd);
		~HttpRequest();

		// SETTERS:
		void set_status_code(const string& _status);
		void set_body(const string& _body);
		bool set_method(const string& _method);
		bool set_url(const string& _url);
		bool set_version(const string& _version);
		bool set_map(const string& _key, const string& _value);
		void set_server(Server _server);
		void set_is_complete(bool _is_complete);
		void set_file_offset(streampos _file_offset);

		// GETTERS:
		const string& get_status_code(void) const;
		const string& get_method(void) const;
		const string& get_url(void) const;
		const string& get_version(void) const;
		const string& get_body(void) const;
		const map<string, string>& get_header(void) const;
		void  display_request(); // TO REMOVE
		Server get_server() const;
		bool  get_is_complete() const;
		int get_serv_socket_communication_fd() const;
		streampos get_file_offset() const;
};

#endif