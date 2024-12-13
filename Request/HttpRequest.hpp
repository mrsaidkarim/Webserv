/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelabbas <zelabbas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 16:00:30 by zelabbas          #+#    #+#             */
/*   Updated: 2024/12/12 17:29:51 by zelabbas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_H
# define HTTPREQUEST_H

#include "../const.hpp"

enum stats {
	START_LINE,
	HEADER,
	BODY
};

class HttpRequest
{
	private:
		string 				method;
		vector<string> 		url;
		string 				version;
		string 				body;
		string				query;
		string				fragment;
		string 				statusCode;
		map<string, string>	header;
		map<string, char>	encoding_symbols;
		int					content_length;
	
	protected:
		void				checkFileType(const string& path);
		void				string_to_lower(string& _str);
		void				append_to_buffer(const string& _content_request); // to implement this for buuffering;
		vector<string>		split(const string& str, char delimiter);
		vector<string>		split_by_string(const string& str, const string& delimiter);
		string				trim_string(const string& str);
		bool				check_url_characters(const string& _url);
		bool				is_valid_characters(const string& str);
		bool				is_valid_value(const string& _value);
		bool				is_start_with_space(const string& str);
		bool				is_valid_header_request(const string& _header);
		bool				check_header_elements();
		bool				parse_body(const string& _body);
		bool				handle_chunked_request(const string& _body); // to implement
		bool				handle_boundary_rquest(const string& _body); // to implement
		const string		get_boundary_key();
		const string		get_name_file(void);
		bool				get_boundary_file_name(map<string, string>& boundary_header, string& file_name);
		bool 				get_boundary_header(const string& part_file, map<string, string>& _header ,string& content_file);
		bool				create_file_and_put_data(const string& file_name, const string& data);	
		public:
		HttpRequest(const string& _request);
		~HttpRequest();
		size_t			hex_to_dic(const string& _hex_str); // from hexa to dicimal
		bool				handle_normal_body(const string& _body);

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

		// GETTERS:
		const string& get_status_code(void) const;
		const string& get_method(void) const;
		const vector<string>& get_url(void) const;
		const string& get_version(void) const;
		const string& get_body(void) const;
		const string& get_query(void) const;
		const string& get_fragment(void) const;
		const map<string, string>& get_header(void) const;
		void  display_request(); // TO REMOVE
};

#endif