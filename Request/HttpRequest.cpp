/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skarim <skarim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 16:00:33 by zelabbas          #+#    #+#             */
/*   Updated: 2025/01/22 10:10:20 by skarim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include <cstddef>
#include <cstdio>
#include <exception>

static string generate_file_name(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    // Extract seconds and microseconds
    time_t rawTime = tv.tv_sec;
    int microseconds = tv.tv_usec;
    
    // Convert to local time
    struct tm *timeInfo = localtime(&rawTime);

    // Create the formatted string
    ostringstream oss;
    oss << CGI_PATH
        << (timeInfo->tm_year + 1900) << "_"       // Full Year (e.g., 2025)
        << (timeInfo->tm_mon + 1) << "_"           // Month
        << timeInfo->tm_mday << "_"               // Day
        << timeInfo->tm_hour << "_"               // Hours
        << timeInfo->tm_min << "_"                // Minutes
        << timeInfo->tm_sec << "_"                // Seconds
        << microseconds << ".txt";               // Microseconds

    return oss.str();
}

void display_first_line_request(const string &line) {
	struct timeval tv;
    gettimeofday(&tv, NULL);
    
    // Extract seconds and microseconds
    time_t rawTime = tv.tv_sec;
    int microseconds = tv.tv_usec;
    
    // Convert to local time
    struct tm *timeInfo = localtime(&rawTime);

    // Create the formatted string
    ostringstream oss;
    oss << timeInfo->tm_hour << "-"               // Hours
        << timeInfo->tm_min << "-"                // Minutes
        << timeInfo->tm_sec << "-"                // Seconds
		<< microseconds;


    string result = line;
    size_t pos = result.find("HTTP/1.1");
    if (pos != string::npos) {
        result = result.substr(0, pos); // Extract the part before "HTTP/1.1"
    }
    cout << BOLD_CYAN << "[" << oss.str() << "]: " << result << endl;
}

void HttpRequest::http_request_init() {
	string			first_line;
	string			header;
	string			body;
	vector<string>	start_line;
	size_t			index;

	cookie = 0;
	was_cgi = false;
	read_content_length = 0;
	file_offset = 0;
	is_chunked = false;
	is_complete = false;
	is_complete_post = false;
	is_cgi = false;
	cgi_in_process = false;
	is_unlink_file_path = false;
	is_cgi_complete = false;
	file_stream = NULL;
	index = request.find(CRLF_2);
	is_binary_post = true;
	chunked_post_offset = 0;
	DEBUG_MODE && cout << BOLD_YELLOW << "HttpRequest constructer" << RESET << "\n";
	DEBUG_MODE && cout << BOLD_YELLOW << request << RESET << "\n";

	if (index == string::npos) {
		this->set_status_code("400");
		goto error;
	}
	header = request.substr(0,index);
	body = request.substr(index + 4);
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
	display_first_line_request(first_line);
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
		if (this->header.find("content-type") == this->header.end()) {
			this->set_status_code("400");
			goto error;
		}
			
		if (this->header.find("content-type")->second == "application/x-www-form-urlencoded") {
			this->is_cgi = true;
			cgi_path_post = generate_file_name();
			DEBUG_MODE && cout << BOLD_RED << cgi_path_post << "\n" << RESET;
			try {
				file_stream = new fstream(cgi_path_post.c_str(), ios::out | ios::trunc | ios::binary);
			} catch (std::exception& e) {
				DEBUG_MODE && cerr << BOLD_RED << "new failed " << e.what() << "\n";
				set_status_code("500");
				goto error;
			}
			if (!file_stream || !file_stream->is_open()) {
				DEBUG_MODE && cerr << "can't open the file\n";
				set_status_code("500");
				goto error;
			}
		} else {
			body = "\r\n" + body;
		}
		check_chunked();
		set_file_path(UPLOAD_SUCCESSFUL);
		if (this->is_chunked)
			body = "\r\n" + body;
		set_body(body);
		set_boundary_key(); 
		if (this->header.find("content-length") == this->header.end() || this->header.find("content-length")->second == "0") {
			this->set_status_code("411");
			goto error;
		}
	}
	content_length = 0;
	if (this->header.find("content-length") != this->header.end() )
	{
		stringStream ss(this->header["content-length"]);
		ss >> content_length;
		if (ss.fail() || (server.get_client_max_body_size() > -1 && content_length > server.get_client_max_body_size()))
		{
			this->set_status_code("413");
			goto error;
		}
	}
	else
		content_length = -1;

	return ;
	error :
		cout << RED << "Error: Malformed request detected\n" << RESET;
		set_is_cgi(false);
		set_is_cgi_complete(true);
		set_is_complete_post(true);
}

HttpRequest::HttpRequest(const string& _request) {
	request = _request;
}

static bool startsWithTmp(const string& path) {
    const string prefix = "/tmp/";
    return path.compare(0, prefix.size(), prefix) == 0;
}

static bool endsWithTxt(const string& path) {
    const string suffix = ".txt";
    if (path.length() >= suffix.length()) {
        return path.compare(path.length() - suffix.length(), suffix.length(), suffix) == 0;
    }
    return false;
}

HttpRequest::~HttpRequest() {
    if (file_stream) {
        file_stream->close();
        delete file_stream;
    }
    if (was_cgi && !cgi_output_file.empty() &&startsWithTmp(cgi_output_file))
        remove(cgi_output_file.c_str());
    if (!cgi_path_post.empty() && startsWithTmp(cgi_path_post) && endsWithTxt(cgi_path_post))
        remove(cgi_path_post.c_str());
    DEBUG_MODE && cout << BOLD_YELLOW << "HttpRequest destructer" << RESET << endl;
}

void	HttpRequest::set_status_code(const string& _status) {
	if (this->statusCode.empty())
		this->statusCode = _status;
}

void	HttpRequest::set_body(const string& _body) {
	this->body = _body;
	read_content_length += _body.size();
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

void HttpRequest::set_path_info(string& _url) {
    const char* extensions[] = {".py", ".php", ".sh"};
    size_t num_extensions = sizeof(extensions) / sizeof(extensions[0]);

    for (size_t i = 0; i < num_extensions; ++i) {
        size_t pos = _url.find(extensions[i]);
        if (pos != string::npos) {
            if ((pos + strlen(extensions[i])) < _url.size() && _url[pos + strlen(extensions[i])] == '/') {
                path_info = _url.substr(pos + strlen(extensions[i]));
                _url = _url.substr(0, pos + strlen(extensions[i]));
            }
            return;
        }
    }
}

bool HttpRequest::set_url(const string& _url) {
	string	update_url = _url;
	string	key;
	size_t query_pos, fragment_pos, path_end;
	if (_url[0] != '/' || !check_url_characters(update_url))
		return (this->set_status_code("400"), false);
	for (size_t i = 0; i < _url.length(); i++)
	{
		if (update_url[i] == '%') {
			key = update_url.substr(i, 3);
			if (encoding_symbols[key] == '\0')
				return (this->set_status_code("400"), false);
			update_url.replace(i, 3, 1,encoding_symbols[key]);
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
	set_path_info(update_url);
	this->url = split(update_url, '/');
	if (this->url.empty())
		url.push_back("/");
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
	if (_key == "host" && ((header.find("host") != header.end()) || _value[0] == ':' || !is_valid_characters(_value) || split(_value, ' ').size() != 1))
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

bool HttpRequest::is_valid_header_request(const string& _header) {
	stringStream	str(_header);
	string			line;
	string			key, value;
	size_t			index;
	while (getline(str, line))
	{
		if (!line.empty() && line.back() == '\r')
			line.pop_back();
		index = line.find(':');
		if (is_start_with_space(line) || index == 0 || 
			index == string::npos || line[index - 1] == ' ')
			return (this->set_status_code("400"), false);
		key = line.substr(0, index);
		value = line.substr(index + 1);
		upper_to_lower(key);
		if (!this->set_map(key, trim_string(value)))
			return (this->set_status_code("400"), false);
	}
	return (true);
}

vector<string> HttpRequest::split(const string& str, char delimiter) {
	vector<string> words;
	string word;
	for (size_t i = 0; i < str.size(); i++) {
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

bool HttpRequest::is_start_with_space(const string& str) {
	if (str[0] == ' ')
		return (true);
	return (false);
}

void HttpRequest::upper_to_lower(string& str) {
	for (size_t i = 0; i < str.length(); i++)
	{
		str[i] = tolower(str[i]);
	}
}

bool HttpRequest::is_valid_characters(const string& str) {
	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] >= 9 && str[i] <= 13)
			return (false);
	}
	return (true);
}

bool HttpRequest::check_url_characters(const string& _url) {
	string valid_characters = "!\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~";
	for (size_t i = 0; i < _url.length(); i++)
	{
		if ((valid_characters.find(_url[i]) == string::npos) && !isalnum(_url[i]))
			return (false);
	}
	return (true);
}

bool HttpRequest::is_valid_value(const string& _value) {
	stringStream	str(_value);
	long 			value;
	if (_value.empty())
		return (this->set_status_code("400"), false);
	str >> value;
	if (!str.eof() || value < 0)
		return (this->set_status_code("400"), false);
	if (str.fail())
		return (this->set_status_code("413"), false);
	return (true);
}

bool HttpRequest::check_header_elements() {
	if (this->header["host"].empty())
		return (this->set_status_code("400"),false);
	if (header.find("content-length") != header.end() && !is_valid_value(this->header["content-length"]))
		return (this->set_status_code("400"),false);
	if (header.find("transfer-encoding") != header.end() && (header["transfer-encoding"] != "chunked"))
		return (this->set_status_code("501"), false);
	return (true);
}


static string add_dollars_before_CRLF(const string &input) {
    string result;
    string word = "\r\n";
    size_t pos = 0;
    size_t found;

    while ((found = input.find(word, pos)) != string::npos) {
        result += input.substr(pos, found - pos);
        result += "{$$$$}";
        result += word;
        pos = found + word.length();
    }
    result += input.substr(pos);

    return result;
}

// display the Request: info
void HttpRequest::display_request() {
	string _url;
	for (size_t i = 0; i < this->url.size(); i++)
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

	cout << "Body: " << add_dollars_before_CRLF(get_body()) << "\n" << RESET;
}

void HttpRequest::set_client_socket(int _client_socket) {
	this->client_socket = _client_socket;
}

int HttpRequest::get_client_socket(void) const {
	return (this->client_socket);
}

void HttpRequest::set_server(const Server& _server) {
	this->server = _server;
}

const Server& HttpRequest::get_server(void) const {
	return (this->server);
}

void HttpRequest::set_is_complete(bool _is_complete) {
	this->is_complete = _is_complete;
}

bool HttpRequest::get_is_complete(void) const {
	return (this->is_complete);
}

void HttpRequest::set_is_chunked(bool _is_chunked) {
	this->is_chunked = _is_chunked;
}

bool HttpRequest::get_is_chunked(void) const {
	return (this->is_chunked);
}

void HttpRequest::set_file_path(const string& _file_path) {
	this->file_path = _file_path;
}

const string& HttpRequest::get_file_path(void) const {
	return (this->file_path);
}

void HttpRequest::set_file_stream(fstream* _file_stream) {
	this->file_stream = _file_stream;
}

fstream* HttpRequest::get_file_stream(void) const {
	return (this->file_stream);
}

void HttpRequest::set_file_offset(streampos _file_offset) {
	this->file_offset = _file_offset;
}

streampos HttpRequest::get_file_offset(void) const {
	return (this->file_offset);
}

void HttpRequest::add_to_body(const string &slice, int byte_read) {
	read_content_length += byte_read;
	body += slice;	
}

bool HttpRequest::set_boundary_key(void) {
	map<string, string>::iterator it = header.find("content-type");
	if (it == header.end()) {
		DEBUG_MODE && cerr << BOLD_RED << "couldn't find content-type" << RESET << "\n";
		return (false);
	}
	size_t pos = it->second.find("boundary=");
	if (pos == string::npos) {
		DEBUG_MODE && cerr << BOLD_RED << "couldn't find boundary key" << RESET << "\n";
		return (false);
	}
	boundary_key = it->second.substr(pos + 9); // 9 is length("boundary=")

	// set boundary key begin 
	boundary_key_begin = CRLF;
	boundary_key_begin += "--";
	boundary_key_begin += boundary_key;
	boundary_key_begin += CRLF;

	// set boundary key end
	boundary_key_end = CRLF;
	boundary_key_end += "--";
	boundary_key_end += boundary_key;
	boundary_key_end += "--";
	boundary_key_end += CRLF;

	DEBUG_MODE && cout << BOLD_GREEN << boundary_key << "\n" << RESET;
	return (true);
}

void HttpRequest::check_chunked()
{
	map<string, string>::iterator it = header.find("transfer-encoding");
	if (it != header.end() && it->second == "chunked")
		this->is_chunked = true;
}



const string& HttpRequest::get_boundary_key_begin(void) const {
	return (boundary_key_begin);
}

const string& HttpRequest::get_boundary_key_end(void) const {
	return (boundary_key_end);
}

bool HttpRequest::get_is_cgi(void) const {
	return is_cgi;
}

void HttpRequest::set_is_cgi(bool is_cgi) {
	this->is_cgi = is_cgi;
} 

bool HttpRequest::get_is_unlink_file_path(void) const {
	return is_unlink_file_path;
}

void HttpRequest::set_is_unlink_file_path(bool is_unlink) {
	this->is_unlink_file_path = is_unlink;
}

const string& HttpRequest::get_cgi_path_post() const {
	return cgi_path_post;
}

void HttpRequest::set_cgi_in_process(bool _cgi_in_process) {
	cgi_in_process = _cgi_in_process;
}

bool HttpRequest::get_cgi_in_process(void) const {
	return cgi_in_process;
}

bool HttpRequest::get_is_complete_post(void) const {
	return is_complete_post;
}

void HttpRequest::set_is_complete_post(bool _is_complete_post) {
	is_complete_post = _is_complete_post;
}

bool HttpRequest::get_was_cgi(void) const {
	return was_cgi;
}

void HttpRequest::set_was_cgi(bool _was_cgi) {
	was_cgi = _was_cgi;
}

void HttpRequest::set_cgi_file_path(const string& _file_path) {
	cgi_output_file = _file_path;
}

void HttpRequest::set_session_id(const string& _session_id) {
	session_id = _session_id;
}

const string& HttpRequest::get_session_id(void) const{
	return(session_id);
}

void HttpRequest::set_cgi_input_file(const string& _cgi_file_input) {
	cgi_input_file = _cgi_file_input;
}

const string& HttpRequest::get_cgi_input_file(void) const {
	return (cgi_input_file);
}

void HttpRequest::set_cgi_path_post(const string& _cgi_path_post) {
	cgi_path_post = _cgi_path_post;
}

void HttpRequest::set_cookie(int _cookie) {
	cookie = _cookie;
}

int HttpRequest::get_cookie(void) const {
	return (cookie);
}

bool HttpRequest::get_is_binary_post(void) const
{
	return (this->is_binary_post);
}

void HttpRequest::set_is_binary_post(bool is_binary_post)
{
	this->is_binary_post = is_binary_post;
}

void HttpRequest::set_content_length(long content_length)
{
	this->content_length = content_length;
}

long HttpRequest::get_content_length(void) const
{
	return (content_length);
}

const string& HttpRequest::get_path_info(void) const {
	return (path_info);
}

void HttpRequest::set_is_cgi_complete(bool _is_cgi_complete) {
	is_cgi_complete = _is_cgi_complete;
}

bool HttpRequest::get_is_cgi_complete(void) const {
	return (is_cgi_complete);
}

string HttpRequest::get_status_line() {
    if (statusCode.empty()) {
        return "HTTP/1.1 200 OK\r\n";
    }
	map<string, string>::const_iterator it = server.get_error_pages().find(statusCode);
    if (statusCode == "400") {
		if (it != server.get_error_pages().end())
			set_file_path(server.get_global_root() + it->second);
		else
			set_file_path(BAD_REQUEST);
        return "HTTP/1.1 400 Bad Request\r\n";
    }
    else if (statusCode == "403") {
		if (it != server.get_error_pages().end())
			set_file_path(server.get_global_root() + it->second);
		else
			set_file_path(FORBIDDEN);
        return "HTTP/1.1 403 Forbidden\r\n";
    }
    else if (statusCode == "404") {
		if (it != server.get_error_pages().end())
			set_file_path(server.get_global_root() + it->second);
		else
			set_file_path(NOT_FOUND);
        return "HTTP/1.1 404 Not Found\r\n";
    }
    else if (statusCode == "405") {
		if (it != server.get_error_pages().end())
			set_file_path(server.get_global_root() + it->second);
		else
			set_file_path(NOT_ALLOWED);
        return "HTTP/1.1 405 Method Not Allowed\r\n";
    }
    else if (statusCode == "408") {
		if (it != server.get_error_pages().end())
			set_file_path(server.get_global_root() + it->second);
		else
			set_file_path(REQUEST_TIMEOUT);
        return "HTTP/1.1 408 Request Timeout\r\n";
    }
    else if (statusCode == "411") {
		if (it != server.get_error_pages().end())
			set_file_path(server.get_global_root() + it->second);
		else
			set_file_path(LENGTH_REQUIRED);
        return "HTTP/1.1 411 Length Required\r\n";
    }
    else if (statusCode == "413") {
		if (it != server.get_error_pages().end())
			set_file_path(server.get_global_root() + it->second);
		else
			set_file_path(TOO_LARGE);
        return "HTTP/1.1 413 Payload Too Large\r\n";
    }
    else if (statusCode == "501") {
		if (it != server.get_error_pages().end())
			set_file_path(server.get_global_root() + it->second);
		else
			set_file_path(NOT_IMPLEMENTED);
        return "HTTP/1.1 501 Not Implemented\r\n";
    }
    else if (statusCode == "505") {
		if (it != server.get_error_pages().end())
			set_file_path(server.get_global_root() + it->second);
		else
			set_file_path(VERSION_NOT_SUPPORTED);
        return "HTTP/1.1 505 HTTP Version Not Supported\r\n";
    }
	if (it != server.get_error_pages().end())
		set_file_path(server.get_global_root() + it->second);
	else
		set_file_path(INTERNAL_SERVER_ERROR);
    return "HTTP/1.1 500 Internal Server Error\r\n";
}

void HttpRequest::set_chunked_post_offset(size_t _chunked_post_offset)
{
	this->chunked_post_offset = _chunked_post_offset;
}

size_t HttpRequest::get_chunked_post_offset(void) const
{
	return (chunked_post_offset);
}
