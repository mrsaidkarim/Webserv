#include "HttpResponse.hpp"
#include <cstddef>
#include <sys/stat.h>

// start delete method!

// check if the file exist
bool HttpResponse::does_not_exist(const string& path) const{
	struct stat statbuf;
	return (stat(path.c_str(), &statbuf) != 0); // Returns true if the file does not exist.
}

// check the path if a file 
bool HttpResponse::is_a_file(const string& path) const {
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf) == 0)
		return S_ISREG(statbuf.st_mode);
	return false; // The path does not exist or is not a regular file.
}

bool HttpResponse::delete_file(const string& filepath) const {
	if (unlink(filepath.c_str()) == 0) {
		cout << "File deleted successfully: " << filepath << "\n"; // TO REMOVE
		request->set_file_path(DEL_SUCCESS); // UPDTE THE path IN CONST.H
		send_response();
		return (true);
	} else {
		cerr << "Error deleting file: " << "Permission denied!" << "\n";// TO REMOVE
		return (false);
	}
}

// check if the path is a dir or a file and also check the permession if the path is a file
void HttpResponse::handle_file(const string& path) const {

	if (does_not_exist(path)) {
		cout << "error the file does not exist!\n"; // TO REMOVE
		request->set_status_code("404");
		send_response();
		return ;
	}
	if (is_a_file(path)) {
		if (access(path.c_str(), W_OK) != 0) {
			cout << "You don't have the write permission for: " << path << "\n"; // TO REMOVE
			request->set_status_code("403");
			send_response();
			return ;
		}
		else {
			if (!delete_file(path)) {
				cout << "error : can't delete the file!\n"; // TO REMOVE
				request->set_status_code("403");
				send_response();
				return ;
			}
		}
	} else {
		cout << path << " is a directory\n"; // TO REMOVE
		request->set_status_code("403");
		send_response();
		return ;
	}
}

// check delete method is allowed or not 
bool	HttpResponse::is_allowed(int index_location, string method) const {
	map<string, bool> methods;
	methods = request->get_server().get_locations()[index_location].get_methods();
	if (methods[method])
		return (true);
	return (false);
}

// zelabbas remove this
bool	HttpResponse::is_allowed(int index_location) const {
	map<string, bool> methods;
	methods = request->get_server().get_locations()[index_location].get_methods();
	if (methods["DELETE"])
		return (true);
	return (false);
}

void HttpResponse::delete_method() const {
	string			path;
	pair<int , int>	longest;
	int				x;
	vector<string>	url;

	cout << "here! start delete\n"; // to remove just for debuging 

	 if (request->get_is_chunked()) {
		send_response();
		return ;
	}

	longest = longest_common_location();
	url = request->get_url();

	if (longest.first != -1) {
		x = longest.first;
		// check if the method allowed in this location if no return method not allowed!
		if (!is_allowed(x)) {
			cout << request->get_server().get_locations()[x].get_root() << "\n";
			cout << request->get_server().get_global_root() << "\n";
			request->set_status_code("405");
			send_response();
			request->set_is_complete(true); // update
			return ;
		}

		path = request->get_server().get_locations()[x].get_root();
		// check if the location has no root, use global root
		if (path.empty())
			path = request->get_server().get_global_root();

		// join the url with the root location 
		for (size_t i = 0; i < url.size(); i++)
		{
			// if (i > 0) // TO Discuss 
			path += "/";
			path += url[i];
		}
		cout << BOLD_GREEN << path << RESET << "\n"; // TO REMOVE
		handle_file(path);
	}
	else {
		cout << "here error!!!!!\n";
		// longest.first == -1;
		// if there's no longest_common_location should return not found ?
		path = request->get_server().get_global_root();

		// if the global root is empty so not found.
		if (path.empty()) {
			request->set_status_code("404");
			send_response();
			return ;
		}

		// join the url with the global root
		for (size_t i = 0; i < url.size(); i++)
		{
			// if (i > 0) // TO Discuss 
			path += "/";
			path += url[i];
		}
		handle_file(path);
	}	
}
// end delete method!