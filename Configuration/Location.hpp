#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "../const.hpp"

class Location
{
    private:
        vector<string> route;
        vector<string> indexes;
        bool auto_index;
        string	root;
		string	location_upload_store;
        map<string, bool> methods;
        pair<string, string> redirections;

		bool	methods_set;
		bool	auto_index_set;
    public:
        Location();
        Location(const vector<string> &route, const vector<string> &indexes, bool auto_index,
                const string &root, const map<string, bool> &methods,
                const pair<string, string> &redirections);
        // Location(string file_path); here do your shit alparser nigro o 9ed setters;
        ~Location();

        // GETTERS
        const vector<string> &get_route(void) const;
        const vector<string> &get_indexes(void) const;
        bool get_auto_index(void) const;
        const string &get_root(void) const;
		const string &get_location_upload_store(void) const;
        const map<string, bool> &get_methods(void) const;
        const pair<string, string> &get_redirections(void) const;

        // SETTERS
        bool	set_route(const vector<string> &route);
        bool	set_indexes(const vector<string> &);
        bool	set_auto_index(bool);
        bool	set_root(const string &);
        bool	set_methods(const map<string, bool> &);
        bool	set_redirections(const pair<string, string> &);
		bool	set_location_upload_store(const string& upload_store);

		bool	does_not_exist(const string& path);
		bool	is_a_file(const string& path);
		bool	check_is_dir(const string& path, int num_server);
        // to remove
        void print_lacation_info(void) const;
};

#endif