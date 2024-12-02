/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelabbas <zelabbas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 16:00:30 by zelabbas          #+#    #+#             */
/*   Updated: 2024/12/02 12:10:37 by zelabbas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_H
# define HTTPREQUEST_H

#include "../const.hpp"

class HttpRequest
{
	private:
		string 				method;
		string 				url;
		string 				version;
		string 				body;
		string				query;
		string				fragment;
		string 				statusCode;
		map<string, string>	header;
		map<string, char>	encoding_symbols;
	
	protected:
		vector<string>	split(const string& str, char delimiter);
		string			trim_string(const string& str);
		bool			check_url_characters(const string& _url);
		bool			is_valid_characters(const string& str);
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

		// GETTERS:
		const string& get_status_code(void) const;
		const string& get_method(void) const;
		const string& get_url(void) const;
		const string& get_version(void) const;
		const string& get_body(void) const;
		const string& get_query(void) const;
		const string& get_fragment(void) const;
		const map<string, string>& get_header(void) const;
		void			display_request(); // TO REMOVE
};

#endif