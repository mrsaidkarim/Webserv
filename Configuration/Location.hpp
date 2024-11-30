#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "../const.hpp"

class Location
{
    private:
        string route;
        vector<string> indexes;
        bool auto_index;
        string root;
        map<string, bool> methods;
        pair<string, string> redirections;
    public:
        Location();
        Location(const string &route, const vector<string> &indexes, bool auto_index,
                const string &root, const map<string, bool> &methods,
                const pair<string, string> &redirections);
        // Location(string file_path); here do your shit alparser nigro o 9ed setters;
        ~Location();

        // GETTERS
        const string &get_route(void) const;
        const vector<string> &get_indexes(void) const;
        bool get_auto_index(void) const;
        const string &get_root(void) const;
        const map<string, bool> &get_methods(void) const;
        const pair<string, string> &get_redirections(void) const;

        // SETTERS
        bool set_route(const string &route);
        bool set_indexes(const vector<string> &);
        bool set_auto_index(bool);
        bool set_root(const string &);
        bool set_methods(const map<string, bool> &);
        bool set_redirections(const pair<string, string> &);

        // to remove
        void print_lacation_info(void) const;
};

#endif