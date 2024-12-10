#include "HttpResponse.hpp"
#include <sys/stat.h>
#include <unistd.h>
#include "../WebServ.hpp"

class WebServ;

HttpResponse::HttpResponse(HttpRequest *_request) {
    this->request = _request;
}

void    HttpResponse::serv() {
    if (request->get_method() == "GET")
        get_method();
    else if (request->get_method() == "POST")
        post_method();
    else if (request->get_method() == "DELETE")
        delete_method();
}

//priority: 1 (redirection)
void HttpResponse::serv_redirection() {
    string httpResponse =
    "HTTP/1.1 " + request->get_server().get_redirection().first + " Moved Permanently\r\n"
    "Location:" + request->get_server().get_redirection().second + "\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Content-Length: 0\r\n"
    "Connection: close\r\n"
    "\r\n";

    write(request->get_serv_socket_communication_fd(), httpResponse.c_str(), httpResponse.size());
    request->set_is_complete(true);
}

void HttpResponse::serv_404() {
    string httpResponse =
    "HTTP/1.1 404 Not Found\r\n"
    "Content-Type: text/html\r\n"
    "Connection: keep-alive\r\n"
    "Content-Length: 389\r\n" // Updated length
    "\r\n"
    "<html>\r\n"
    "<head>\r\n"
    "    <title>404 Not Found</title>\r\n"
    "    <style>\r\n"
    "        body {\r\n"
    "            background-color: black;\r\n"
    "        }\r\n"
    "        h1 {\r\n"
    "            color: white;\r\n"
    "            text-align: center;\r\n"
    "        }\r\n"
    "    </style>\r\n"
    "</head>\r\n"
    "<body>\r\n"
    "    <center><h1>404 Not Found</h1></center>\r\n"
    "    <center><img src=\"https://i.ytimg.com/vi/4m8zqpu7VdM/sddefault.jpg\" alt=\"404 Image\"></center>\r\n"
    "</body>\r\n"
    "</html>\r\n";
    write(request->get_serv_socket_communication_fd(), httpResponse.c_str(), httpResponse.size());
    request->set_is_complete(true);
}



vector<string> HttpResponse::split_url(const string &s) {
    stringStream ss(s);
    string slice;
    vector<string> splited_url;

    while (getline(ss, slice, '/')) {
        if (!slice.empty())
            splited_url.push_back(slice);
    }

    // for (int i = 0; i < splited_url.size(); i++) {
    //     cout << splited_url[i] << ",";
    // }
    // cout << "\n";
    
    return (splited_url);
}

pair<int, int> HttpResponse::longest_common_location() {
    vector<string> route = split_url(request->get_url());
    // cout << "splited_route -->";
    // for (int i = 0; i < route.size();i++) {
    //     cout << " " << route[i];
    // }
    // cout << endl;
    int best_i = -1;
    int best_j;

    for (int i = 0; i < request->get_server().get_locations().size(); i++) {
        int j = 0;
        while (j < request->get_server().get_locations()[i].get_route().size() && j < route.size()) {
            if (request->get_server().get_locations()[i].get_route()[j] != route[j])
                break;
            j++;
        }

        if (j > 0) {
            if (best_i == -1 || best_j < j) {
                best_i = i;
                best_j = j;
            }
        }
    }

    return (make_pair(best_i, best_j));
}


string to_hex(int number) {
    ostringstream oss;
    oss << hex << number; // Convert to hex
    return oss.str();
}

string HttpResponse::get_content_type(const string &path) {
    unordered_map<string, string> extensions;

    extensions.insert(make_pair("html", "text/html"));
    extensions.insert(make_pair("css", "text/css"));
    extensions.insert(make_pair("js", "text/javascript"));
    extensions.insert(make_pair("jpg", "image/jpeg"));
    extensions.insert(make_pair("jpeg", "image/jpeg"));
    extensions.insert(make_pair("png", "image/png"));
    extensions.insert(make_pair("gif", "image/gif"));
    extensions.insert(make_pair("bmp", "image/bmp"));
    extensions.insert(make_pair("ico", "image/x-icon"));
    extensions.insert(make_pair("svg", "image/svg+xml"));
    extensions.insert(make_pair("mp4", "video/mp4"));
    extensions.insert(make_pair("webm", "video/webm"));
    extensions.insert(make_pair("ogg", "video/ogg"));
    extensions.insert(make_pair("mp3", "audio/mpeg"));
    extensions.insert(make_pair("wav", "audio/wav"));
    extensions.insert(make_pair("pdf", "application/pdf"));
    extensions.insert(make_pair("zip", "application/zip"));
    extensions.insert(make_pair("tar", "application/x-tar"));
    extensions.insert(make_pair("gz", "application/gzip"));
    extensions.insert(make_pair("bz2", "application/x-bzip2"));
    extensions.insert(make_pair("7z", "application/x-7z-compressed"));
    extensions.insert(make_pair("txt", "text/plain"));
    extensions.insert(make_pair("rtf", "application/rtf"));
    extensions.insert(make_pair("doc", "application/msword"));
    extensions.insert(make_pair("docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"));
    extensions.insert(make_pair("xls", "application/vnd.ms-excel"));
    extensions.insert(make_pair("xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"));
    extensions.insert(make_pair("ppt", "application/vnd.ms-powerpoint"));
    extensions.insert(make_pair("pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"));
    extensions.insert(make_pair("csv", "text/csv"));
    extensions.insert(make_pair("xml", "application/xml"));
    extensions.insert(make_pair("json", "application/json"));
    extensions.insert(make_pair("py", "text/x-python"));
    extensions.insert(make_pair("c", "text/x-csrc"));
    extensions.insert(make_pair("cpp", "text/x-c++src"));
    extensions.insert(make_pair("h", "text/x-chdr"));
    extensions.insert(make_pair("hpp", "text/x-c++hdr"));
    extensions.insert(make_pair("java", "text/x-java"));
    extensions.insert(make_pair("sh", "application/x-sh"));
    extensions.insert(make_pair("bat", "application/x-bat"));
    extensions.insert(make_pair("exe", "application/x-msdownload"));
    extensions.insert(make_pair("bin", "application/octet-stream"));
    extensions.insert(make_pair("deb", "application/x-deb"));
    extensions.insert(make_pair("rpm", "application/x-rpm"));


    size_t pos = path.rfind('.');
    if (pos != string::npos) {
        string extension = path.substr(pos + 1);
        if (extensions.find(extension) != extensions.end()) {
            return (extensions[extension]);
        }
    }
    return ("text/plain");
}


void HttpResponse::get_response_html(string &path) {
    fstream file(path.c_str(), ios::in);
    if (!file) {
        serv_404();
        return;
    }

    string content_type = get_content_type(path);
    // cout << BOLD_MAGENTA << content_type << "\n";

    // Update Content-Type to match your HTML content
    if (request->get_file_offset() == 0) {
        string http_response_header =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: " + content_type + "\r\n"
            "Transfer-Encoding: chunked\r\n"
            "\r\n";
        write(request->get_serv_socket_communication_fd(), http_response_header.c_str(), http_response_header.size());
    }

    file.seekg(request->get_file_offset());
    // cout << BOLD_YELLOW << "prev offset: " << request->get_file_offset() << RESET << endl;
    char buffer[BUFFER_SIZE];
    // struct stat file_status;
    if (file.read(buffer, BUFFER_SIZE) || file.gcount() > 0) {
        size_t bytes_read = file.gcount();
        string chunk_size = to_hex(bytes_read);  // Assuming to_hex is working fine
        chunk_size += "\r\n";

        // Write chunk size
        write(request->get_serv_socket_communication_fd(), chunk_size.c_str(), chunk_size.size());
        // Write actual chunk data
        write(request->get_serv_socket_communication_fd(), buffer, bytes_read);
        // End of chunk
        write(request->get_serv_socket_communication_fd(), "\r\n", 2);
    }


    // Check if we have reached the end of the file
    request->set_file_offset(file.tellg());
    // cout << BOLD_YELLOW << "cur offset: " << request->get_file_offset() << RESET << endl;
    if (file.tellg() == -1) {
        // Final chunk with 0 size
        write(request->get_serv_socket_communication_fd(), "0\r\n\r\n", 5);  // Ends the chunked response
        request->set_is_complete(true);
    }
    
    file.close();
}



// void HttpResponse::get_response_html(string &path) {
//     fstream file(path, ios::in);
//     if (!file) {
//         serv_404();
//         return ;
//     }

//     // read all file in http_body
//     string http_body; 
//     string buffer;

//     while (getline(file, buffer)) {
//         if (!http_body.empty())
//             http_body += "\r\n";
//         http_body += buffer;
//     }

//     // create http response
//     string httpResponse =
//     "HTTP/1.1 200 OK\r\n"
//     "Content-Length: " + to_string(http_body.size()) + "\r\n"
//     "Content-Type: text/html\r\n"
//     "Connection: Closed\r\n"
//     "\r\n" + http_body;

//     write(fd, httpResponse.c_str(), httpResponse.size());
//     file.close();
// }

string generateAutoindexHTML(const string& dirPath, const string& basePath) {
    string html = R"(<!DOCTYPE html>
<html lang="en">
<head>
       <title>404 Not Found</title>
       <style>
           body {
               background-color: black;
           }
           h1 {
               color: white;
               text-align: center;
           }
       </style>
   </head>
<body>
    <h1><a href="../" class="back-link">..</a></h1>
    )";

    DIR* dir = opendir(dirPath.c_str());
    if (!dir) {
        html += "<h1>Error: Unable to open directory</h1>";
    } else {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            string name = entry->d_name;

            // Skip "." and ".."
            if (name == "." || name == "..") {
                continue;
            }

            string fullPath = dirPath + "/" + name;

            // Get file metadata
            struct stat fileStat;
            if (stat(fullPath.c_str(), &fileStat) == -1) {
                continue; // Skip this file if stat failed
            }
            // Use the basePath to generate absolute links
            html += "<h1><a href='" + basePath + "/" + name;
            if (S_ISDIR(fileStat.st_mode)) {
                html += "/";
            }
            html += "'>" + name + "</a></h1>";
        }
        closedir(dir);
    }

    html += R"(
</body>
</html>)";

    return html;
}

void    HttpResponse::get_response_autoindex(string &html_body) {
    string httpResponse =
    "HTTP/1.1 200 OK\r\n"
    "Content-Length: " + to_string(html_body.size()) + "\r\n"
    "Content-Type: text/html\r\n"
    "Connection: Closed\r\n"
    "\r\n" + html_body;

    write(request->get_serv_socket_communication_fd(), httpResponse.c_str(), httpResponse.size());
    request->set_is_complete(true);
}

void HttpResponse::get_method() {
    //priority: 1 (redirection)
    if (!request->get_server().get_redirection().first.empty()) {
        serv_redirection();
        return;
    }

    //priority: 2 (location)
    pair<int, int> longest = longest_common_location();
    if (longest.first != -1) {
        // cout << "location should handle this shit\n";
        int x = longest.first;
        int y = longest.second;

        vector<string> route = split_url(request->get_url());

        // check for root
        string path = request->get_server().get_locations()[x].get_root();
        // if not root for this location use global root
        if (path.empty())
            path = request->get_server().get_global_root();
        
        // join the url with root
        for (int i = 0; i < route.size(); i++) {
            path += "/" + route[i];
        }

        // cout << "this is path : " << path << "\n";
        // check if path is file or directory
        struct stat path_status;

        // cout << "path : " << path << "\n";
        if (stat(path.c_str(), &path_status) == -1) {
            cerr << "stat failed\n";
            // cout << "her\n";
            serv_404();
            return ;

        } else if (S_ISDIR(path_status.st_mode)) {
            string index_path;
            // cout << "this is directory : " << path << "\n";
            for (int i = 0; i < request->get_server().get_locations()[x].get_indexes().size(); i++) {
                string index = request->get_server().get_locations()[x].get_indexes()[i];
                index_path = path + "/" + index;
                if (stat(index_path.c_str(), &path_status) == 0) {
                    get_response_html(index_path);
                    return ;
                }
            }

            index_path = path + "/index.html";
            if (stat(index_path.c_str(), &path_status) == 0) {
                get_response_html(index_path);
                return ;
            }

            if (request->get_server().get_locations()[x].get_auto_index()) {
                string html_body = generateAutoindexHTML(path, request->get_url());
                get_response_autoindex(html_body);
                return ;
            }
            serv_404();
            // here we should check for index
        } else if (S_ISREG(path_status.st_mode)) {
            // here we should check for file type
            // for now I will just response to html files
            get_response_html(path);
            return ;
        } else {
            serv_404();
            return ;
        }
    }

    if (split_url(request->get_url()).size() > 2) {
        // cout << "no location found\n";
        serv_404();
        return ;
    }


    struct stat path_status;
    string path = request->get_server().get_global_root();
    string index_path;
    if (request->get_url() != "/") {
        index_path = path + request->get_url();
        if (stat(index_path.c_str(), &path_status) == 0) {
            get_response_html(index_path);
            return ;
        } else {
            serv_404();
            return ;
        }
    }   


    //priority: 3 (index)
    path = request->get_server().get_global_root();;
    for (int i = 0; i < request->get_server().get_indexes().size(); i++) {
        // cout << "hna\n";
        string index = request->get_server().get_indexes()[i];
        index = path + "/" + index;
        if (stat(index_path.c_str(), &path_status) == 0) {
            get_response_html(index_path);
            return ;
        }        
    }

    index_path = path + "/index.html";
    if (stat(index_path.c_str(), &path_status) == 0) {
        get_response_html(index_path);
        return ;
    }
    
    //priority: 4 (autoindex)
    if (request->get_server().get_autoindex()) {
        string html_body = generateAutoindexHTML(path, request->get_url());
        get_response_autoindex(html_body);
        return ;
    }
    serv_404();
}

void HttpResponse::post_method() {

}

void HttpResponse::delete_method() {

}

HttpResponse::~HttpResponse() {
	
}

HttpRequest *HttpResponse::get_request() const {
    return (this->request);
}