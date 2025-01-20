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
        vector<int> ports;
		vector<string> server_names;
		long long client_max_body_size;
		vector<Location> locations;
		string global_root;
		string host_name;
		pair<string, string> redirection;
		vector<string> indexes;
		bool autoindex;
		map<string, string> error_pages;

		bool	autoindex_set;
    public:
        Server();
		Server(const Server& _server);

		Server& operator=(const Server& _server);
        // Server(const vector<int> &ports, const vector<string> &server_names, const long long &client_max_body_size,
		// 		const vector<Location> &locations, const string &global_root, const pair<string, string> &redirection,
		// 		const vector<string> &indexes, bool autoindex, const map<string, string> &error_pages);
        
        // GETTERS
		vector<pair<int, string> > get_ports_hosts(void) const;
		const vector<string> &get_server_names(void) const;
		const long long &get_client_max_body_size(void) const;
		const vector<Location> &get_locations(void) const;
		const string &get_global_root(void) const;
		const pair<string, string> &get_redirection(void) const;
		const vector<string> &get_indexes(void) const;
		bool  get_autoindex(void) const;
		const string& get_global_upload_store(void) const;
		const map<string, string> &get_error_pages(void) const;
		const string&	get_host_name(void) const;

		// SETTERS
		void	set_locations(Location& location);
		bool	set_host_name(const string& _host_name);
		void	set_error_pages(const string& key, const string& value); // to handle!
		void	set_ports(int port);
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
		bool	check_attributes_server(); // to implement check the important attributes!

		// to remove
		void print_server_info(void) const;
        // GETTERS
        // const vector<int> &get_ports(void) const;
        // const vector<string> &get_server_names(void) const;
        // const long long &get_client_max_body_size(void) const;
        // const vector<Location> &get_locations(void) const;
        // const string &get_global_root(void) const;
        // const pair<string, string> &get_redirection(void) const;
        // const vector<string> &get_indexes(void) const;
        // bool get_autoindex(void) const;
        // const vector<string> &get_error_pages(void) const;

        // // SETTERS


        // // to remove
        // void print_server_info(void) const;
        
};

#endif