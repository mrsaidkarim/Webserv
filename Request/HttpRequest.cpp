/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelabbas <zelabbas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 16:00:33 by zelabbas          #+#    #+#             */
/*   Updated: 2024/12/11 14:31:42 by zelabbas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"


// handle delete method here!
bool	deleteFile(const string& filepath) {
	if (unlink(filepath.c_str()) == 0) {
		cout << "File deleted successfully: " << filepath << "\n";
		return (true);
	} else {
		cerr << "Error deleting file: " << "Permission denied!" << "\n";
		return (false);
	}
}

void HttpRequest::checkFileType(const string& path) {
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf) == 0) {
		if (S_ISREG(statbuf.st_mode)) {
			cout << path << " is a file\\n\n";
			if (access(path.c_str(), W_OK) != 0)
				cout << "You don't have the write permission for: " << path << "\n\n\n";
			else {
				if (!deleteFile(path))
					cout << "error : can't delete the file!\n";
			}
		} else if (S_ISDIR(statbuf.st_mode)) {
			cout << path << " is a directory\n\n\n";
		} else {
			cout << path << " is neither a file nor a directory\n";
		}
	} else {
		cout << "error the file does not exist\n";
	}
}
// handle delete method here!

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
	this->set_encoding_symbols();
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
	if (!is_valid_header_request(header) || !check_header_elements())
		goto error;
	if (this->get_method() == "POST") {
		if (!this->parse_body(this->get_body())) {
			goto error;
		}
	}
	if (this->get_method() == "DELETE")
	{
		cout << BOLD_YELLOW << "handle Delete Method\n" << RESET;
		string path = "/Users/zelabbas/Webserv";	
		for (int i = 0; i < this->url.size(); i++)
		{
			path += "/" + url[i];
		}
		checkFileType(path);
	}
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
	string	update_url = _url;
	string	key;
	size_t query_pos, fragment_pos, path_end;
	if (_url[0] != '/' || !check_url_characters(update_url))
		return (this->set_status_code("400"), false);
	for (int i = 0; i < _url.length(); i++)
	{
		if (update_url[i] == '%') {
			key = update_url.substr(i, 3);
			if (encoding_symbols[key] == '\0')
				return (this->set_status_code("400"), false);
			 update_url.replace(i, 3, 1, encoding_symbols[key]);
		}
	}
	query_pos =  update_url.find('?');
	fragment_pos = update_url.find('#');
	path_end = std::min(query_pos, fragment_pos);
	if (path_end == string::npos) path_end = _url.length();
	if (query_pos != string::npos && (fragment_pos == string::npos || query_pos < fragment_pos))
		set_query(update_url.substr(query_pos + 1, fragment_pos - query_pos - 1));
	if (fragment_pos != string::npos)
		set_fragment(update_url.substr(fragment_pos + 1));
	update_url = update_url.substr(0, path_end);
	this->url = split(update_url, '/');
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
	if (_key == "Host" && ((header.find("Host") != header.end()) || _value[0] == ':' || !is_valid_characters(_value) || split(_value, ' ').size() != 1))
		return (this->set_status_code("400"), false);
	this->header[_key] = _value;
	return (true);
}

void HttpRequest::set_fragment(const string& _fragment) {
	this->fragment = _fragment;
}

void HttpRequest::set_encoding_symbols() {
	this->encoding_symbols["%20"] = ' ';
	this->encoding_symbols["%21"] = '!';
	this->encoding_symbols["%22"] = '"';
	this->encoding_symbols["%23"] = '#';
	this->encoding_symbols["%24"] = '$';
	this->encoding_symbols["%25"] = '%';
	this->encoding_symbols["%26"] = '&';
	this->encoding_symbols["%27"] = '\'';
	this->encoding_symbols["%28"] = '(';
	this->encoding_symbols["%29"] = ')';
	this->encoding_symbols["%2A"] = '*';
	this->encoding_symbols["%2B"] = '+';
	this->encoding_symbols["%2C"] = ',';
	this->encoding_symbols["%2D"] = '-';
	this->encoding_symbols["%2E"] = '.';
	this->encoding_symbols["%2F"] = '/';
	this->encoding_symbols["%3A"] = ':';
	this->encoding_symbols["%3B"] = ';';
	this->encoding_symbols["%3C"] = '<';
	this->encoding_symbols["%3D"] = '=';
	this->encoding_symbols["%3E"] = '>';
	this->encoding_symbols["%3F"] = '?';
	this->encoding_symbols["%40"] = '@';
	this->encoding_symbols["%5B"] = '[';
	this->encoding_symbols["%5C"] = '\\';
	this->encoding_symbols["%5D"] = ']';
	this->encoding_symbols["%5E"] = '^';
	this->encoding_symbols["%5F"] = '_';
	this->encoding_symbols["%60"] = '`';
	this->encoding_symbols["%7B"] = '{';
	this->encoding_symbols["%7C"] = '|';
	this->encoding_symbols["%7D"] = '}';
	this->encoding_symbols["%7E"] = '~';
}

void HttpRequest::set_query(const string& _query) {
	this->query = _query;
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

const vector<string>& HttpRequest::get_url(void) const {
	return (this->url);
}

const string& HttpRequest::get_body(void) const {
	return (this->body);
}

const map<string, string>& HttpRequest::get_header(void) const {
	return (this->header);
}

const string& HttpRequest::get_query(void) const {
	return (this->query);
}

const string& HttpRequest::get_fragment(void) const {
	return (this->fragment);
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

vector<string>  HttpRequest::split_by_string(const string& str, const string& delimiter) {
	vector<string> parts;
	size_t start = 0;
	size_t end;
	while ((end = str.find(delimiter, start)) != string::npos) {
		parts.push_back(str.substr(start, end - start));
		start = end + delimiter.length();
	}
	parts.push_back(str.substr(start));
	return parts;
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

size_t HttpRequest::hex_to_dic(const string& _hex_str) {
	size_t value;
	stringStream hx_str(_hex_str);
	hx_str >> std::hex >> value;
	if (hx_str.fail() || !hx_str.eof())
		throw std::invalid_argument("Error: invalid Hex Number\n");
	return (value);
}

bool HttpRequest::is_start_with_space(const string& str) {
	if (str[0] == ' ')
		return (true);
	return (false);
}

// SHOUDL CHECK ALL WHITSPACESS NOT JUST TAB
bool HttpRequest::is_valid_characters(const string& str) {
	for (int i = 0; i < str.length(); i++)
	{
		// if (str[i] == '\t')
		if (str[i] >= 9 && str[i] <= 13)
			return (false);
	}
	return (true);
}

bool HttpRequest::check_url_characters(const string& _url) {
	string valid_characters = "!\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~";
	for (int i = 0; i < _url.length(); i++)
	{
		if ((valid_characters.find(_url[i]) == string::npos) && !isalnum(_url[i]))
			return (false);
	}
	return (true);
}

bool HttpRequest::is_valid_value(const string& _value) {
	stringStream	str(_value);
	int 			value;
	if (_value.empty())
		return (this->set_status_code("400"), false);
	str >> value;
	if (!str.eof() || value < 0)
		return (this->set_status_code("400"), false);
	if (str.fail())
		return (this->set_status_code("413"), false);
	this->content_length = value;
	return (true);
}

bool HttpRequest::check_header_elements() {
	if (this->header["Host"].empty())
		return (this->set_status_code("400"),false);
	if (header.find("Content-Length") != header.end() && !is_valid_value(this->header["Content-Length"]))
		return (this->set_status_code("400"),false);
	if (header.find("Transfer-Encoding") != header.end() && (header["Transfer-Encoding"] != "chunked"))
		return (this->set_status_code("501"), false);
	return (true);
}

bool HttpRequest::parse_body(const string& _body) {
	if (this->header.find("Transfer-Encoding") != header.end()) {
		// if (handle_chunked_request(_body)) // this will return true or false
			return (true);
	}
	else if ((this->header.find("Content-Type") != header.end()) && split(header["Content-Type"], ' ').size() >= 2) {
		if (!handle_boundary_rquest(body)) { // this also will return true or false
			return (false);
		}

	}
	else {
		if (!handle_normal_body(_body)) // handle normal body using content-length;
			return (false); 
	}
	return  (true);
}

bool HttpRequest::handle_chunked_request(const string& _body) {
	return (true);
}

bool HttpRequest::handle_boundary_rquest(const string& _body) {
	string						boundary_key;
	string						tmp_str;
	map<string, string>			boundary_header;
	string						content_file;
	string						file_name;
	vector<string>				files_part;
	size_t						pos;

	boundary_key = get_boundary_key();
	if (boundary_key == "bad_request")
		return (this->set_status_code("400"), false);
	cout << "the key is :" << boundary_key << "\n"; // ! to remove 
	pos = _body.find(boundary_key + "--");
	if (pos == string::npos)
		return (this->set_status_code("400"), false);
	tmp_str = body.substr(0, pos - 2);
	cout << BOLD_YELLOW << tmp_str << "\n";
	files_part = split_by_string(tmp_str, "--" + boundary_key + "\r\n");
	for (int i = 0; i < files_part.size(); i++)
	{
		cout << BOLD_RED  << i << " :" << files_part[i] << "\n" RESET;
		if (!files_part[i].empty()) {
			if (!get_boundary_header(files_part[i], boundary_header, content_file)) 
			return (this->set_status_code("400"), false);
		if (!get_boundary_file_name(boundary_header, file_name)) {
			cout << "here !5\n";
			return (this->set_status_code("400"), false);
		}
		if (!create_file_and_put_data(file_name, content_file))
			return (this->set_status_code("500"), false);
		}
	}
	return (true);
}

bool HttpRequest::handle_normal_body(const string& _body) {
	std::ofstream	outFile("/tmp/" + get_name_file() + ".txt");
	size_t			len;
	if (outFile.fail())
		return (this->set_status_code("500"), false); // server side error
	len = this->content_length;
	for (size_t i = 0; i < len; i++)
	{
		outFile << this->body[i];
	}
	outFile.close();
	return (true);
}

const string HttpRequest::get_name_file(void) {
	static size_t	i;
	string			sufix;
	string			name;

	sufix = "tmp_file";
	name = (sufix + std::to_string(i));
	i++;
	return (name);
}

const string HttpRequest::get_boundary_key() {
	string prefix = "multipart/form-data; boundary=";
	if (this->header["Content-Type"].find(prefix) == 0)
		return (this->header["Content-Type"].substr(prefix.length()));
	return ("bad_request");
}

bool HttpRequest::get_boundary_file_name(map<string, string>& boundary_header, string& file_name) {
	vector<string>	values;
	size_t			pos;
	if (boundary_header.find("Content-Disposition") == boundary_header.end())
		return (this->set_status_code("400"), false);
	values = split(boundary_header["Content-Disposition"], ' ');
	for (int i = 0; i < values.size(); i++)
	{
		pos = values[i].find("filename=");
		if (pos != string::npos) {
			file_name = values[i].substr(pos + 9);
			if (file_name == "\"\"")
				return (this->set_status_code("400"), false);
			file_name = file_name.substr(1, file_name.length() - 2);
			return (true);
		}
	}
	return (false);
}

bool HttpRequest::create_file_and_put_data(const string& file_name, const string& data) {
	std::ofstream	outFile("/Users/zelabbas/Webserv/Upload/" + file_name);
	if (outFile.fail())
		return (this->set_status_code("500"), false);
	outFile << data;
	outFile.close();
	return (true);
}

// !! example how to parse the post method body!
    // std::string boundary = "----------------------------850594822365976215450362";
    // std::string body = "----------------------------850594822365976215450362\r\n"
    //                    "Content-Disposition: form-data; name=\"file\"; filename=\"input.csv\"\r\n"
    //                    "Content-Type: text/csv\r\n\r\n"
    //                    "Received request: date | value\n"
    //                    "2005-02-02 | 0.1\n"
    //                    "2011-09-14 | 390.57\n"
    //                    "2010-11-03 | 517.239\n"
    //                    "...\r\n"
    //                    "----------------------------850594822365976215450362--";

bool	HttpRequest::get_boundary_header(const string& part_file, map<string, string>& _header, string& content_file) {
	string				tmp_str;
	string				line;
	size_t				pos;

	// cout << BOLD_YELLOW << "here >> " << part_file << "\n";
	pos = part_file.find(CRLF_2);
	if (pos == string::npos)
		return (this->set_status_code("400"), false);
	tmp_str = part_file.substr(0, pos);
	content_file = part_file.substr(pos + 4);
	stringStream ss(tmp_str);
	while (std::getline(ss, line))
	{
		if (!line.empty() && line.back() == '\r')
			line.pop_back();
		pos = line.find(':');
		if (is_start_with_space(line) || pos == 0 || 
			pos == string::npos || line[pos - 1] == ' ')
			return (this->set_status_code("400"), false);
		_header[line.substr(0, pos)] = trim_string(line.substr(pos + 1));
	}
	return (true);
}

// display the Request: info
void HttpRequest::display_request() {
	string _url;
	for (int i = 0; i < this->url.size(); i++)
	{
		_url += "/" + url[i];
	}
	cout << GREEN << "Method using: " << get_method() << "\n" << RESET;
	cout << BLUE << "URL: " << _url << "\n" << RESET;
	cout << BOLD_CYAN << "Query: " << get_query() << "\n" << RESET;
	cout << BOLD_GREEN << "Fragment: " <<  get_fragment() << "\n" << RESET;
	cout << YELLOW << "Version HTTP: " << get_version() << "\n" << RESET;

	for (map<string, string>::iterator it = header.begin(); it != header.end(); it++)
	{
		cout << CYAN << it->first << ": " << it->second << "\n";
	}
	cout << RESET;

	cout << BG_MAGENTA << "Body: " << get_body() << "\n" << RESET;
}

//  if (it->second.find_first_not_of("!#$&'()*+,/:;=?@[]-_.~ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") != std::string::npos)
// request-target longer than any URI it wishes to parse MUST respond
//    with a 414 (URI Too Long)

//  header-field   = field-name ":" OWS field-value OWS

//      field-name     = token
//      field-value    = *( field-content / obs-fold )
//      field-content  = field-vchar [ 1*( SP / HTAB ) field-vchar ]
//      field-vchar    = VCHAR / obs-text

//      obs-fold       = CRLF 1*( SP / HTAB )
//                     ; obsolete line folding
//                     ; see Section 3.2.4

//  transfer-coding    = "chunked" ; Section 4.1
//                         / "compress" ; Section 4.2.1
//                         / "deflate" ; Section 4.2.2
//                         / "gzip" ; Section 4.2.3
//                         / transfer-extension