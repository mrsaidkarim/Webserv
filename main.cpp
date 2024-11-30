/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skarim <skarim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 12:06:33 by skarim            #+#    #+#             */
/*   Updated: 2024/11/30 12:28:00 by skarim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "const.hpp"
#include "Configuration/Location.hpp"

int main(void)
{
    string route = "/";
    vector<string> indexes = {"index.html", "home.html"};
    bool auto_index = true;
    string root = "/var/www/example";
    map<string, bool> methods = {
        {"GET", true},
        {"POST", true},
        {"DELETE", false}
    };
    pair<string, string> redirections = {"301", "www.youtube.com"};

    // Create a Location object
    Location Location(route, indexes, auto_index, root, methods, redirections);
    Location.print_lacation_info();
}