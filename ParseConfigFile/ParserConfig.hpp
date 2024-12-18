#ifndef PARSER_CONFIG_HPP
#define PARSER_CONFIG_HPP

#include "../const.hpp"
#include "../WebServ.hpp"

class ParserConfig
{
	private:
		/* data */
	vector<string>		directive_server;
	vector<string>		directive_location;
	stack<char>			stack_brackets;
	int					port;
	// long long			max_body_size;
	map<string, bool>		methods;
	pair<string, string>	redirections;
	ifstream				config_file;
	bool					good_config;
	// bool					start_server;
	protected:
		void			init_directive();
		bool			open_file(const string& path);
		bool			check_start_server(const string& line);
		bool			check_start_location(const vector<string>& vec);
		bool			is_valid_extension(const string& path);
		void			trim_line(string& line);
		vector<string>	split(const string& str, char delimiter1, char delimiter2);
		bool			parse_line(string& line);
		bool			handle_server(WebServ& webserver, const string& leftover_line);
		bool			handle_location(Server& server, const vector<string>& vec);

		bool			set_directive_location(const vector<string>& vec, const vector<string>::iterator& it, Location& loc);
		bool			set_directive_server(const vector<string>& vec, const vector<string>::iterator& it, Server& server);

		bool			check_line_server(string& line);
		bool			check_line_location(string& line);
		bool			check_autoindex(const vector<string>& vec);
		bool			check_allow_methods(const vector<string>& vec);
		bool			check_upload_store(const vector<string>& vec);
		bool			check_listen(const vector<string>& vec);
		bool			check_return(const vector<string>& vec);
	public:
		ParserConfig(const string& path, WebServ& webserv);
		~ParserConfig();

		bool	is_good_config();
};
#endif