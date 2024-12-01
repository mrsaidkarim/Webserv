/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelabbas <zelabbas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 16:00:33 by zelabbas          #+#    #+#             */
/*   Updated: 2024/12/01 15:31:15 by zelabbas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

HttpRequest::HttpRequest(const string& _request) {
	string			first_line;
	string			header;
	string			body;
	vector<string>	start_line;
	size_t			index;

	cout << "HttpRequest constructer\n";
	index = _request.find(CRLF_2);
	if (index == string::npos) {
		this->set_status_code("400");
		goto error;
	}
	header = _request.substr(0,index);
	body = _request.substr(index + 4);
	this->set_body(body);
	index = header.find(CRLF);
	if (index == string::npos) {
		this->set_status_code("400");
		goto error;
	}
	first_line = header.substr(0,index);
	header = header.substr(index + 2);
	if (is_start_with_space(first_line) || !is_valid_characters(first_line) || is_start_with_space(header)) {
		this->set_status_code("400");
		goto error;
	}
	start_line = split(first_line, ' ');
	if (start_line.size() != 3) {
		this->set_status_code("400");
		goto error;
	}
	if (!this->set_method(start_line[0]) || !this->set_url(start_line[1]) || !this->set_version(start_line[2]))
		goto error;
	
	// start parse header
	if (!is_valid_header_request(header))
		goto error;
	// end parse header
	// print the result
	// cout << BOLD_GREEN << first_line << "\n" << RESET;
	// cout << BOLD_YELLOW << header << "\n" << RESET;
	// cout << BOLD_BLUE << body << RESET;
	// print the result
	return ;
	error :
		cout << RED << "Error: Malformed request detected\n" << RESET;
}

HttpRequest::~HttpRequest() {
	cout << "HttpRequest destructer\n";
}

// SETTERS:
void	HttpRequest::set_status_code(const string& _status) {
	if (this->statusCode.empty())
		this->statusCode = _status;
}

void	HttpRequest::set_body(const string& _body) {
	this->body = _body;
}

bool HttpRequest::set_method(const string& _method) {
	if (_method != "GET" && _method != "POST" && _method != "DELETE" &&
		_method != "PUT" && _method != "PATCH" && _method != "HEAD"  &&
		_method != "OPTIONS" && _method != "TRACE" && _method != "CONNECT")
			return (this->set_status_code("400") , false);
	else if (_method != "GET" && _method != "POST" && _method != "DELETE")
		return (this->set_status_code("501"), false);
	this->method = _method;
	return (true);
}

bool HttpRequest::set_url(const string& _url) {
	this->url = _url;
	return (true);
}

bool HttpRequest::set_version(const string& _version) {
	string	sufix;
	float	version_value;
	if (_version.length() >= 5) {
		if (_version == "HTTP/1.1")
			return (this->version = _version, true);
		else {
			sufix = _version.substr(0, 5);
			if (sufix != "HTTP/")
				return (this->set_status_code("400") , false);
			stringStream value(_version.substr(5));
			value >> version_value;
			if (value.fail() || !value.eof())
				return (this->set_status_code("400") , false);
		}
		return (this->set_status_code("505") , false);
	}
	return (this->set_status_code("400"), false);
}

bool HttpRequest::set_map(const string& _key, const string& _value) {
	if (!is_valid_characters(_key) || split(_key, ' ').size() != 1)
		return (this->set_status_code("400"), false);
	if (_key == "Host" && (_value[0] == ':' || !is_valid_characters(_value) || split(_value, ' ').size() != 1))
		return (this->set_status_code("400"), false);
	this->header[_key] = _value;
	return (true);
}

// GETTERS
const string& HttpRequest::get_status_code(void) const {
	return (this->statusCode);
}

const string& HttpRequest::get_method(void) const {
	return (this->method);
}

const string& HttpRequest::get_version(void) const {
	return (this->version);
}

const string& HttpRequest::get_url(void) const {
	return (this->url);
}

const string& HttpRequest::get_body(void) const {
	return (this->body);
}

const map<string, string>& HttpRequest::get_header(void) const {
	return (this->header);
}

// utils :
bool HttpRequest::is_valid_header_request(const string& _header) {
	stringStream	str(_header);
	string			line;
	string			key, value;
	size_t			index;
	while (std::getline(str, line))
	{
		if (!line.empty() && line.back() == '\r')
			line.pop_back();
		index = line.find(':');
		if (is_start_with_space(line) || index == 0 || 
			index == string::npos || line[index - 1] == ' ')
			return (this->set_status_code("400"), false);
		key = line.substr(0, index);
		value = line.substr(index + 1);
		if (!this->set_map(key, trim_string(value)))
			return (this->set_status_code("400"), false);
		// cout << line << "\n";
	}
	// cout << RED <<"\n is valid header\n" << RESET;
	return (true);
}

vector<string> HttpRequest::split(const string& str, char delimiter) {
	vector<string> words;
	string word;
	for (int i = 0; i < str.size(); i++) {
		if (str[i] == delimiter) {
			if (!word.empty()) {
				words.push_back(word);
			}
			word.clear();
		} else {
			word += str[i];
		}
	}
	if (!word.empty()) {
		words.push_back(word);
	}
	return words;
}

string HttpRequest::trim_string(const string& str) {

	int start = 0;
	int end = str.size() - 1;

	while (start <= end && std::isspace(str[start]))
		start++;
	while (end >= 0 && std::isspace(str[end]))
		end--;
	return (str.substr(start, end - start + 1));
}


bool	HttpRequest::is_start_with_space(const string& str) {
	if (str[0] == ' ')
		return (true);
	return (false);
}

bool	HttpRequest::is_valid_characters(const string& str) {
	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] == '\t')
			return (false);
	}
	return (true);
}

// display the Request: info
void	HttpRequest::display_request() {
	cout << GREEN << "Method using: " << get_method() << "\n" << RESET;
	cout << BLUE << "URL: " << get_url() << "\n" << RESET;
	cout << YELLOW << "Version HTTP: " << get_version() << "\n" << RESET;

	for (map<string, string>::iterator it = header.begin(); it != header.end(); it++)
	{
		cout << CYAN << it->first << ": " << it->second << "\n";
	}
	cout << RESET;

	cout << BG_MAGENTA << "Body: " << get_body() << "\n" << RESET;
}
