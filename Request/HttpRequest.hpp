/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelabbas <zelabbas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 16:00:30 by zelabbas          #+#    #+#             */
/*   Updated: 2024/11/30 18:13:42 by zelabbas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_H
# define HTTPREQUEST_H

#include "../const.hpp"

class HttpRequest
{
	private:
		string method;
		string url;
		string version;
		string body;
		string statusCode;
		map<string, string> header;

	public:
		HttpRequest(const string& _request);
		~HttpRequest();

		// SETTERS:
		void set_status_code(const string& _status);
		bool set_method(const string& method);
		bool set_url(const string& url);
		bool set_version(const string& version);
		bool set_map(const string& key, const string& value);

		// GETTERS:
		string& get_status_code(void) const;
		string& get_method(void) const;
		string& get_url(void) const;
		string& get_body(void);
		map<string, string> get_header(void) const;
};

#endif