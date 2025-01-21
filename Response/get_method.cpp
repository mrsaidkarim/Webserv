#include "HttpResponse.hpp"
#include <cstddef>
#include <exception>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <unistd.h>

string to_hex(int number) {
    ostringstream oss;
    oss << hex << number; // Convert to hex
    return oss.str();
}

string HttpResponse::get_content_type(const string &path) const {
    unordered_map<string, string> extensions;

    extensions.insert(make_pair("html", "text/html"));
    extensions.insert(make_pair("css", "text/css"));
    extensions.insert(make_pair("js", "application/javascript"));
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
    return ("text/html");
}

//priority: 1 (redirection)
void HttpResponse::serv_redirection(string redirect_code, string redirect_url)  const {

    if (redirect_url.find("http://") != 0 && redirect_url.find("https://") != 0) {
        redirect_url = "http://" + redirect_url;
    }

   string httpResponse =
        "HTTP/1.1 " + redirect_code + " Moved Permanently\r\n"
        "Location: " + redirect_url + "\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "Content-Length: 0\r\n"
        "Connection: close\r\n"
        "\r\n";

    send(request->get_client_socket(), httpResponse.c_str(), httpResponse.size(), 0);
    request->set_is_complete(true);
}



void HttpResponse::serv_autoindex(const string& path) const {
        string body_content =
            "<!DOCTYPE html>"
            "<html lang=\"en\">"
            "<head>"
            "<meta charset=\"UTF-8\">"
            "<title>Autoindex</title>"
            "<style>"
            "body {"
            "background-color: black;"
            "color: white;"
            "}"
            "a {"
            "color: white;"
            "}"
            "h1 {"
            "color: white;"
            "text-align: center;"
            "}"
            "</style>"
            "</head>"
            "<body>"
            "<h1><a href=\"../\" class=\"back-link\">..</a></h1>";

    DIR* dir = opendir(path.c_str());
    if (!dir) {
        request->set_status_code("403");
        send_response();
        return;
    } else {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            string name = entry->d_name;

            // Skip "." and ".."
            if (name == "." || name == "..") {
                continue;
            }

            string fullPath = path + "/" + name;

            string url;
            for (size_t i = 0; i < request->get_url().size(); i++) {
                url += "/";
                url += request->get_url()[i];
            }

            // Get file metadata
            struct stat fileStat;
            if (stat(fullPath.c_str(), &fileStat) == -1) {
                continue; // Skip this file if stat failed
            }
            // Use the basePath to generate absolute links
            body_content += "<h1><a href='" + url + "/" + name;
            if (S_ISDIR(fileStat.st_mode)) {
                body_content += "/";
            }
            body_content += "'>" + name + "</a></h1>";
        }
        closedir(dir);
    }


    body_content += "</body></html>";

    string response =   "HTTP/1.1 200 OK\r\n"
                        "Content-Length: " + to_string(body_content.size()) + "\r\n"
                        "Content-Type: text/html\r\n"
                        "Connection: Closed\r\n";

    cout << "here (!request->get_session_id().empty())1\n\n\n";
    if (!request->get_session_id().empty() && request->get_cookie() != 0) {
        response += "Set-Cookie: session_id_" + to_string(request->get_cookie()) + "=" + request->get_session_id() + "; Path=/; HttpOnly\r\n";
        cout << "here (!request->get_session_id().empty())2\n\n\n";
        cout << request->get_session_id() << "\n\n\n";
    }
    response += "\r\n" + body_content;

    send(request->get_client_socket(), response.c_str(), response.size(), 0);
    request->set_is_complete(true);
}



void HttpResponse::send_response() const{
    cout << BOLD_GREEN << "in send response\n" << RESET;
    if (is_cgi()) {
        cout << BOLD_RED << "is cgi\n" << RESET;
        cgi();
        return;
    }
    if (request->get_file_offset() == 0) {
        cout << "here >>>>>>> 1000\n";
        cout << request->get_file_path() << "\n";
        string status_line = request->get_status_line();
        string path = request->get_file_path();
        // if already file opened close it
        if (request->get_file_stream() && request->get_file_stream()->is_open()) {
            request->get_file_stream()->close();
            delete request->get_file_stream();
            request->set_file_stream(NULL);
        }

        fstream *file;
        try {
            file = new fstream(path.c_str(), ios::in);
        } catch (std::exception& e) {
            cerr << "new failed " << e.what() << "\n";
            serv_500();
            return;
        }
        if (!file->is_open()) {
            delete file;
            request->set_file_stream(NULL);
            serv_404();
            return;
        }

        request->set_file_stream(file);
        string content_type = get_content_type(path);
        string http_response_header =
            "Content-Type: " + content_type + "\r\n"
            "Transfer-Encoding: chunked\r\n"
            "connection: keep-alive\r\n"
            "Accept-Ranges: none\r\n";

        http_response_header = status_line + http_response_header;

        if (!request->get_session_id().empty() && request->get_cookie() != 0) {
            http_response_header += "Set-Cookie: session_id_" + to_string(request->get_cookie()) + "=" + request->get_session_id() + "; Path=/; HttpOnly\r\n";
            cout << "here (!request->get_session_id().empty())\n\n\n";
            cout << request->get_session_id() << "\n\n\n";

        }
        http_response_header += "\r\n";
        if (send(request->get_client_socket(), http_response_header.c_str(), http_response_header.size(), 0) < 0) {
            perror("1) send failed in send_response()");
            request->set_is_complete(true);
            if (file->is_open()) {
                file->close();
            }
            delete file;
            request->set_file_stream(NULL);
            return;
        }
        request->set_is_chunked(true);

    }
    if (!request->get_file_stream())
        return ;

    fstream *file = request->get_file_stream();

    file->seekg(request->get_file_offset());
    // // cout << BOLD_YELLOW << "prev offset: " << request->get_file_offset() << RESET << endl;
    char buffer[BUFFER_SIZE];
    // struct stat file_status;
    if (file->read(buffer, BUFFER_SIZE) || file->gcount() > 0) {
        size_t bytes_read = file->gcount();
        string chunk_size = to_hex(bytes_read);  // Assuming to_hex is working fine
        chunk_size += "\r\n";

        // Write chunk size
        if (send(request->get_client_socket(), chunk_size.c_str(), chunk_size.size(), 0) < 0) {
            perror("2) send failed in send_response()");
            request->set_is_complete(true);
            if (file && file->is_open()) {
                file->close();
            }
            delete file;
            request->set_file_stream(NULL);
            return;
        }
        // Write actual chunk data
        if (send(request->get_client_socket(), buffer, bytes_read, 0) < 0) {
            perror("3) send failed in send_response()");
            request->set_is_complete(true);
            if (file->is_open()) {
                file->close();
            }
            delete file;
            request->set_file_stream(NULL);
            return ;
        }
        // End of chunk
        // write(request->get_client_socket(), "\r\n", 2);
        if (send(request->get_client_socket(), "\r\n", 2, 0) < 0) {
            perror("4) send failed in send_response()");
            request->set_is_complete(true);
            if (file->is_open()) {
                file->close();
            }
            delete file;
            request->set_file_stream(NULL);
            return ;
        }
    }
    // cout << "hello >>>>>>2222000000\n";
    request->set_file_offset(file->tellg());
    // cout << BOLD_YELLOW << "cur offset: " << request->get_file_offset() << RESET << endl;
    // Check if we have reached the end of the file
    if (file->eof()) {
        // Final chunk with 0 size
        send(request->get_client_socket(), "0\r\n\r\n",5 , 0);  // Ends the chunked response
        request->set_is_complete(true);
        if (file->is_open()) {
            file->close();
        }
        delete file;
        request->set_file_stream(NULL);
    }
}

pair<int, int> HttpResponse::longest_common_location() const{
    vector<string> route = request->get_url();
    cout << "splited_route -->";
    for (size_t i = 0; i < route.size();i++) {
        cout << " " << route[i];
    }
    cout << endl;
    int best_i = -1;
    int best_j;

    for (size_t i = 0; i < request->get_server().get_locations().size(); i++) {
        size_t j = 0;
        while (j < request->get_server().get_locations()[i].get_route().size() && j < route.size()) {
            if (request->get_server().get_locations()[i].get_route()[j] != route[j])
                break;
            j++;
        }

        if (j > 0) {
            if (best_i == -1 || best_j < (int)j) {
                best_i = i;
                best_j = j;
            }
        }
    }

    return (make_pair(best_i, best_j));
}


void HttpResponse::get_method() {
    // we already checked if the request is chunked
    // so we continue sending the response
    if (request->get_is_chunked()) {
        send_response();
        return ;
    }

    //priority: 1 (redirection)
    if (!request->get_server().get_redirection().first.empty()) {
        string redirect_code = request->get_server().get_redirection().first;
        string redirect_url = request->get_server().get_redirection().second;
        serv_redirection(redirect_code, redirect_url);
        return;
    }

    //priority: 2 (location)
    pair<int, int> longest = longest_common_location();
    if (longest.first != -1) {
        index_location = longest.first;
        cout << "location should handle this shit\n";
        int x = longest.first;

        // check redirection
        if (!request->get_server().get_locations()[x].get_redirections().first.empty()) {
            string redirect_code = request->get_server().get_locations()[x].get_redirections().first;
            string redirect_url = request->get_server().get_locations()[x].get_redirections().second;
            serv_redirection(redirect_code, redirect_url);
            return;
        }

        vector<string> route = request->get_url();

        // check for root
        string path = request->get_server().get_locations()[x].get_root();
        // if not root for this location use global root
        if (path.empty())
            path = request->get_server().get_global_root();
        
        cout << BOLD_GREEN << path << RESET << endl;
        // join the url with root
        for (size_t i = 0; i < route.size(); i++) {
            if (i > 0)
                path += "/";
            path += route[i];
        }

        cout << BOLD_GREEN << path << RESET << endl;
        // cout << "this is path : " << path << "\n";
        // check if path is file or directory
        struct stat path_status;

        // cout << "path : " << path << "\n";
        if (stat(path.c_str(), &path_status) == -1) { // check file if exist
            // not found
            cerr << "stat failed\n";
            request->set_status_code("404");
            send_response();
            return ;

        } 

        // check if file
        if (S_ISREG(path_status.st_mode)) {
            if (access(path.c_str(), R_OK) < 0)
                request->set_status_code("403");
            else
                request->set_file_path(path);
            send_response();
            return ;
        }

        // check if dir
        if (S_ISDIR(path_status.st_mode)) {
            // check permission
            if (access(path.c_str(), X_OK | R_OK) < 0) {
                request->set_status_code("403");
                send_response();
                return;
            }

            //priority: 3 (index)
            string index_path;
            for (size_t i = 0; i < request->get_server().get_locations()[x].get_indexes().size(); i++) {
                string index = request->get_server().get_locations()[x].get_indexes()[i];
                index_path = path + "/" + index;
                if (stat(index_path.c_str(), &path_status) == 0 && access(index_path.c_str(), R_OK) == 0) {
                    request->set_file_path(index_path);
                    send_response();
                    return ;
                }
            }

            //priority: 4 (autoindex)
            if (request->get_server().get_locations()[x].get_auto_index()) {
                cout << "priority: 4 (autoindex)\n";
                serv_autoindex(path);
                return ;
            }
        }
        request->set_status_code("404");
        send_response();
        return;
    }

    // join url with global root
    string path = request->get_server().get_global_root();
    for (size_t i = 0; i < request->get_url().size(); i++) {
        path += "/" + request->get_url()[i];
    }

    cout << "this is joined path : " << path << endl;

    // check if exist
    struct stat status;
    if (stat(path.c_str(), &status) < 0) {
        request->set_status_code("404");
        send_response();
        return;
    }

    // check if file
    if (S_ISREG(status.st_mode)) {
        // check permission
        if (access(path.c_str(), R_OK) < 0)
            request->set_status_code("403");    
        else
            request->set_file_path(path);
        send_response();
        return ;
    }

    // check if dir
    if (S_ISDIR(status.st_mode)) {
        // check permission
        if (access(path.c_str(), X_OK | R_OK) < 0) {
            request->set_status_code("403");
            send_response();
            return;
        }

        //priority: 3 (index)
        cout << "priority: 3 (index)\n"; 
        string index;
        struct stat file_stat;
        cout << "size : " << request->get_server().get_indexes().size() << "\n";
        for (size_t i = 0; i < request->get_server().get_indexes().size(); i++) {
            index = request->get_server().get_indexes()[i];
            index = path + "/" + index;
            cout << index << "!!!\n";
            if (stat(index.c_str(), &file_stat) == 0 && access(index.c_str(), R_OK) == 0) {
                request->set_file_path(index);
                send_response();
                return ;
            }        
        }

        cout << "priority: 4 (autoindex)\n";
        //priority: 4 (autoindex)
        if (request->get_server().get_autoindex()) {
            serv_autoindex(path);
            return;
        }

    }

    request->set_status_code("404");
    send_response();
}
