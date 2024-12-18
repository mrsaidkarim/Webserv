#include "ParserConfig.hpp"

ParserConfig::ParserConfig(const string& path, WebServ& webserv)
{
	cout << "Constructer Parser!\n";
	string	line;
	good_config = true;
	int start = 0;
	if (!is_valid_extension(path) || !open_file(path)) {
		good_config = false;
		goto bad_config;
	}
	init_directive();
	while (getline(config_file, line)) {
		if (line.empty() || line[0] == '#')
			continue;
		trim_line(line);
		if (!parse_line(line)) {
			good_config = false;
			cout << "here 400\n";
			goto bad_config;
		}
		if (start == 0 && !check_start_server(line)) {
			good_config = false;
			cout << "here 401\n";
			goto bad_config;
		}
		if (start == 0)
			line = "";
		start = 1;
		if (!handle_server(webserv, line)) {
			good_config = false;
			cout << "here 402\n";
			goto bad_config;
		}
	}

	// check if all {} are good
	if (!stack_brackets.empty()) {
		good_config = false;
		cout << "here 403\n";
		goto bad_config;
	}
	return ;
	bad_config:
		cout << BOLD_RED << "Bad config file!\n" << RESET;
}

ParserConfig::~ParserConfig()
{
	if (config_file.is_open())
		config_file.close();
	cout << "Constructer Parser!\n";
}

// protected :

// helps method
// server === > "listen", "server_name", "error_page", "client_max_body_size", "root", "index", "autoindex", "return"
 // location =====> "autoindex", "allow_methods", "return", "php-cgi", "root", "index", "py-cgi", "upload_store"
void ParserConfig::init_directive() {

	// DIRCTIVE SERVER
	directive_server.push_back("listen");
	directive_server.push_back("server_name");
	directive_server.push_back("root");
	directive_server.push_back("index");
	directive_server.push_back("autoindex");
	directive_server.push_back("error_page");
	directive_server.push_back("client_max_body_size");
	directive_server.push_back("return");

	// DIRCTIVE LOCATION
	directive_location.push_back("autoindex");
	directive_location.push_back("allow_methods");
	directive_location.push_back("root");
	directive_location.push_back("index");
	directive_location.push_back("upload_store");
	directive_location.push_back("return");
}

bool ParserConfig::open_file(const string& path) {
	config_file.open(path);
	if (!config_file.is_open()) {
		cerr << BOLD_RED << "Error: Failed to open the file: " << path << RESET << endl;
		return (false);
	}
	return (true);
}

bool ParserConfig::is_valid_extension(const string& path) {
	int pos;

	pos = path.rfind('.');
	if (pos != string::npos) {
		if (path.substr(pos + 1) == "conf")
			return (true);
		else {
			cout << BOLD_RED << "invalid extension: " << path << RESET << endl;
			return (false);
		}
	}
	cout << BOLD_RED << "invalid extension: " << path << RESET << endl;
	return (false);
}

bool ParserConfig::is_good_config() {
	return (this->good_config);
}

bool ParserConfig::check_start_server(const string& line) {
	vector<string> vec;

	vec = split(line, ' ', '\t');
	if (vec.size() != 2)
		return false;
	else {
		if (vec[0] != "server" || vec[1] != "{")
			return (false);
	}
	return (true);
}

bool ParserConfig::check_start_location(const vector<string>& vec) {

	if (vec.size() < 3)
		return (false);
	if (vec[vec.size() - 1] != "{")
		return (false);
	for (int i = 1; i < vec.size() - 1; i++)
	{
		if ((vec[i].find('{') != string::npos) || 
			(vec[i].find('}') != string::npos) || (vec[i].find(';') != string::npos))
				return (false);
		if (vec[i] == "location")
			return (false);
	}
	return (true);
}

bool ParserConfig::parse_line(string& line) {
	int pos;
	if (line.empty())
		return (true);
	if (line[0] == ';')
		return (false);
	pos = line.find('#');
	if (pos != string::npos) {
		line = line.substr(0, pos);
		trim_line(line);
		if (line.empty())
			return (true);
	}
	for (int i = 0; i < line.length(); i++)
	{
		if (line[i] == '}') {
			if (stack_brackets.empty())
				return (false);
			else
				stack_brackets.pop();
		}
		if (line[i] == '{')
			stack_brackets.push(line[i]);
	}
	// cout << GREEN << line + '\n' << RESET;
	return (true);
}

bool ParserConfig::handle_server(WebServ& webserver, const string& leftover_line) {
	Server	server;
	string	line;
	vector<string>::iterator it;
	vector<string> split_line;

	line = leftover_line;
	cout << BOLD_RED << "start new server !>>\n" << RESET;
	while (true) {
		if (line.empty() || line[0] == '#') {
			if (!getline(config_file, line)) break;
			continue;
		}
		trim_line(line);
		if (!parse_line(line))
			return (false);
		// if (line.empty())
		// 	continue ;
		if (!check_line_server(line))
			return (false);
		split_line = split(line, ' ', '\t');
		if (line.empty())
			continue ;
		// cout << ">>>> " << split_line[0] << "\n";
		it = find(directive_server.begin(), directive_server.end(), split_line[0]);
		if (it == directive_server.end() && split_line[0] != "location" && split_line[0] != "}")
		{
			if (find(directive_location.begin(), directive_location.end(), split_line[0]) == directive_location.end()) {
				// cout << BOLD_BLUE << line << RESET;
				if (!check_start_server(line))
					return (false);
				if (stack_brackets.size() != 1) {
					cout << "ma3t!\n";
					return (false);
				}
				// push the server in webservers!
				webserver.set_servers(server);
				server.print_server_info();
				return (true);
			}
		}
		if (split_line[0] == "location") {
			if (!check_start_location(split_line)) {
				cout << "here 404\n";
				return (false);
			}
			if (!handle_location(server, split_line)) {
				cout << "here 405\n";
				return (false);
			}
		}
		else if (it != directive_server.end() && !set_directive_server(split_line, it, server))
			return (false);
		if (!getline(config_file, line))
			break;
	}
	// push the server in webservers!
	cout << "here 1!\n";
	server.print_server_info();
	webserver.set_servers(server);
	return (true);
}

bool ParserConfig::check_line_server(string& line) {
	int pos;
	if (line.empty() || line == "}")
		return (true);
	if ((line.find("location") != string::npos) || (line.find("server") != string::npos)) {
		if (line.find("location") != 0 && line.find("server") != 0)
			return (false);
		return (true);
	}
	else if ((line.find('{') != string::npos) || line.find('}') != string::npos)
		return (false);
	pos = line.find(';');
	if (pos == string::npos)
		return (false);
	else {
		if (line[pos + 1] != '\0')
			return (false);
	}
	line = line.substr(0, pos);
	return (true);
}

bool ParserConfig::handle_location(Server& server, const vector<string>& vec) {
	Location					location;
	string						line;
	vector<string>				split_line;
	vector<string>::iterator	it;

	location.set_route(vec);
	while (getline(config_file, line))
	{
		if (line.empty() || line[0] == '#')
			continue ;
		// cout << BOLD_YELLOW << "location line >>>> " << line << "\n" << RESET;
		trim_line(line);
		if (!parse_line(line) || !check_line_location(line))
			return (false);
		if (line == "}") {
			server.set_locations(location);
			return (true);
		}
		if (line.empty())
			continue ;
		split_line = split(line, ' ', '\t');
		it = find(directive_location.begin(), directive_location.end(), split_line[0]);
		if (it == directive_location.end())
			return (false);
		if (!set_directive_location(split_line, it, location))
			return (false);
	}
	// PUSH location in the server!
	// server.set_locations(location);
	cout << "here fin location!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
	return (true);
}

bool ParserConfig::set_directive_server(const vector<string>& vec, const vector<string>::iterator& directive, Server& server) {
	if (*directive == "listen") {
		if (!check_listen(vec))
			return (false);
		server.set_ports(port);
	}
	if (*directive == "server_name") {
		if ((vec.size() < 2) || !server.set_server_names(vec))
			return (false);
	}
	if (*directive == "root") {
		if ((vec.size() != 2) || !server.set_global_root(vec[1]))
			return (false);
	}
	if (*directive == "index") {
		if ((vec.size() < 2) || !server.set_indexes(vec))
			return (false);
	}
	if (*directive == "autoindex") {
		if (!check_autoindex(vec) || !server.set_autoindex(vec[1] == "on"))
			return (false);
	}
	if (*directive == "return") {
		if (!check_return(vec) || !server.set_redirection(redirections))
			return (false);
	}
	if (*directive == "client_max_body_size") {
		if ((vec.size() != 2) || !server.set_client_max_body_size(vec[1]))
			return (false);
	}
	if (*directive == "error_page") {
		if (vec.size() < 2)
			return (false);
		cout << "error page!\n";
	}
	return (true);
}


bool ParserConfig::set_directive_location(const vector<string>& vec, const vector<string>::iterator& directive, Location& loc) {
	if (*directive == "autoindex") {
		if (!check_autoindex(vec) || !loc.set_auto_index(vec[1] == "on"))
			return ( cout << "1\n" ,false);
	}
	if (*directive == "root") {
		if ((vec.size() != 2) || !loc.set_root(vec[1])) // check if the root is a dir and exist ! in set_root!
			return ( cout << "2\n" ,false);
	}
	if (*directive == "index") {
		if ((vec.size() == 1) || !loc.set_indexes(vec)) // vector<string>(vec.begin() + 1, vec.end());
			return ( cout << "3\n" ,false);
	} 
	if (*directive == "allow_methods") {
		if (!check_allow_methods(vec) || !loc.set_methods(methods))
			return ( cout << "4\n" ,false);
	}
	if (*directive == "upload_store") {
		if ((vec.size() != 2) || !check_upload_store(vec))
			return ( cout << "5\n" ,false);
		// to add upload store in location
	}
	if (*directive == "return") {
		if (!check_return(vec) || !loc.set_redirections(redirections))
			return ( cout << "6\n" ,false);
	}
	return (true);
}

bool ParserConfig::check_listen(const vector<string>& vec) {
	port = -1;
	if (vec.size() != 2)
		return (false);
	stringStream ss(vec[1]);
	ss >> port;
	if (ss.fail() || !ss.eof())
		return (false);
	if (port <= 0 || port > 65535)
		return (false);
	return (true);
}

bool ParserConfig::check_return(const vector<string>& vec) {
	int value;
	if (vec.size() != 3)
		return (false);
	redirections.first = vec[1];
	redirections.second = vec[2];
	stringStream ss(vec[1]);
	ss >> value;
	if (ss.fail() || !ss.eof())
		return (false);
	if (value >= 300 && value <= 399)
		return (true);
	else
		return (false);
}



bool ParserConfig::check_autoindex(const vector<string>& vec) {
	if (vec.size() != 2)
		return (false);
	if (vec[1] != "on" && vec[1] != "off")
		return (false);
	return (true);
}

bool ParserConfig::check_allow_methods(const vector<string>& vec) {
	if (vec.size() == 1 || vec.size() >= 5)
		return (false);
	methods["GET"] = true;
	methods["POST"] = false;
	methods["DELETE"] = false;
	for (int i = 1; i < vec.size(); i++)
	{
		if (vec[i] != "GET" && vec[i] != "POST" && vec[i] != "DELETE") 
		{
			// cout << BOLD_RED << "DABRTI 3ELIHA : " <<  vec[i] << RESET;
			return (false);
		}
		if (vec[i] == "POST")
			methods["POST"] = true;
		if (vec[i] == "DELETE")
			methods["DELETE"] = true;
	}
	return (true);
}

bool ParserConfig::check_upload_store(const vector<string>& vec) {

	// should check if is a dir and exist ? 
	return (true);
}


bool ParserConfig::check_line_location(string& line) {
	int pos;
	// cout << "here check line >> " << line << '\n';
	if (line.empty()  || line == "}")
		return (true);
	if (line[line.length() - 1] != ';')
		return (false);
	line = line.substr(0, line.length() - 1);
	for (int i = 0; i < line.length(); i++)
	{
		if (line[i] == '{' || line[i] == '}')
			return (false);
		if (line[i] == ';')
			return (false);
	}
	return (true);
}

void ParserConfig::trim_line(string& line) {

	int start = 0;
	int end = line.size() - 1;

	while (start <= end && std::isspace(line[start]))
		start++;
	while (end >= 0 && std::isspace(line[end]))
		end--;
	line = line.substr(start, end - start + 1);
}


vector<string> ParserConfig::split(const string& str, char delimiter1, char delimiter2) {
	vector<string> words;
	string word;
	for (int i = 0; i < str.size(); i++) {
		if (str[i] == delimiter1 || str[i] == delimiter2) {
			if (!word.empty()) {
				words.push_back(word);
			}
			word.clear();
		} else {
			word += str[i];
		}
	}
	if (!word.empty())
		words.push_back(word);
	return words;
}