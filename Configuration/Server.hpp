/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skarim <skarim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 12:39:27 by skarim            #+#    #+#             */
/*   Updated: 2024/11/30 12:54:42 by skarim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "../const.hpp"
#include "Location.hpp"

class Server
{
    private:
        vector<pair<int, string> > ports;
		vector<string> server_names;
		long client_max_body_size;
		vector<Location> locations;
		string global_root;
		pair<string, string> redirection;
		vector<string> indexes;
		bool autoindex;
		map<string, string> error_pages;

		bool	autoindex_set;
    public:
        Server();
		Server(const Server& _server);

		Server& operator=(const Server& _server);
        
        // GETTERS
		const  vector<pair<int, string> > &get_ports(void) const;
		const vector<string> &get_server_names(void) const;
		const long &get_client_max_body_size(void) const;
		const vector<Location> &get_locations(void) const;
		const string &get_global_root(void) const;
		const pair<string, string> &get_redirection(void) const;
		const vector<string> &get_indexes(void) const;
		bool  get_autoindex(void) const;
		const string& get_global_upload_store(void) const;
		const map<string, string> &get_error_pages(void) const;

		// SETTERS
		void	set_locations(Location& location);
		void	set_error_pages(const string& key, const string& value);
		void	set_ports(int port,const string& _host);
		bool	set_global_root(const string& root);
		bool	set_server_names(const vector<string> & vec);
		bool	set_indexes(const vector<string>& vec);
		bool	set_autoindex(bool auto_index);
		bool	set_redirection(const pair<string, string>& _redirection);
		bool	set_client_max_body_size(const string& str_value);
		bool	set_global_upload_store(const string& upload_stor);
		
		bool	does_not_exist(const string& path);
		bool	is_a_file(const string& path);
		bool	check_is_dir(const string& path, int num_server);
		bool	check_attributes_server();

		void print_server_info(void) const;
};

#endif
