/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   post_method.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zech-chi <zech-chi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/15 12:54:20 by skarim            #+#    #+#             */
/*   Updated: 2025/01/21 22:07:21 by zech-chi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include <exception>

bool is_crlf_exist_more_than_five_times(const string &s) {
    int count = 0;
    size_t pos = s.find(CRLF);

    while (count < 5 && pos != string::npos) {
        count++;
        pos += 2; // 2 is size of CRLF
        pos = s.find(CRLF, pos);
    }

    return (count == 5);
}

static string addPrefixBeforeCRLF(const string &input) {
    const string word = "\r\n";
    const string prefix = "{$$$$}";
    string result = input;
    size_t pos = 0;

    while ((pos = result.find(word, pos)) != string::npos) {
        result.insert(pos, prefix);
        pos += prefix.size() + word.size(); // Move past the added prefix and word
    }

    return result;
}

size_t convert_chunk_size(const string &s)
{
    char *end_ptr;
    size_t result = strtoul(s.c_str(), &end_ptr, 16);
    if (*end_ptr != '\0')
        throw invalid_argument("Invalid hexa number: " + s);
    return result;
}

bool HttpResponse::normalize_chunked_data(string &s) {
    const string crlf = "\r\n";
    size_t pos = request->get_chunked_post_offset(); // Current position in the string
    if (pos > s.size())
    {
        request->set_chunked_post_offset(pos - s.size());
        return true;   
    }
    while (pos < s.size() && (pos = s.find(crlf, pos)) != string::npos) {
        size_t chunk_start = pos + crlf.size(); // Start of potential chunk size
        size_t i = chunk_start;
        // Check if the chunk size is a valid hexadecimal number
        while (i < s.size() && isxdigit(s[i])) {
            i++;
        }

        if (i + 2 > s.size())
            return (false);
        // Ensure the detected chunk size is followed by CRLF
        if (i > chunk_start && i + 1 < s.size() && s[i] == '\r' && s[i + 1] == '\n') {
            // Extract the chunk size as a hexadecimal number
            string chunk_size_hex = s.substr(chunk_start, i - chunk_start);
            size_t chunk_size;
            try
            {
                chunk_size = convert_chunk_size(chunk_size_hex);
            }
            catch(const invalid_argument& e)
            {
                DEBUG_MODE && cerr << e.what() << '\n';
                request->set_is_complete_post(true);
                request->set_is_cgi(false);
                request->set_is_cgi_complete(true);
                request->set_status_code("500");
                return (false);
            }
            // Check if the body contains the full chunk data
            size_t chunk_data_start = i + crlf.size(); // Start of the chunk data
            size_t chunk_data_end = chunk_data_start + chunk_size; // End of the chunk data
            if (chunk_size == 0)
            {
                s.erase(pos);
                return (true);
            }
            if (chunk_data_end > s.size()) {
                // Incomplete chunk
                request->set_chunked_post_offset(chunk_size - s.size() + chunk_data_start);
                s.erase(pos, chunk_data_start - pos);
                return true;
            }
            s.erase(pos, chunk_data_start - pos);
            // Reset position to start searching from the new content
            pos += chunk_size;
        } else {
            // Move past CRLF if no valid chunk size is found
            pos += crlf.size();
        }
    }
    request->set_chunked_post_offset(0);
    return (true);
}

static string get_file_extension(const string& content_type) {
    map<string, string> mime_to_ext;

    mime_to_ext["/html"] = ".html";
    mime_to_ext["/plain"] = ".txt";
    mime_to_ext["/css"] = ".css";
    mime_to_ext["/javascript"] = ".js";
    mime_to_ext["/json"] = ".json";
    mime_to_ext["/xml"] = ".xml";
    mime_to_ext["/pdf"] = ".pdf";
    mime_to_ext["/zip"] = ".zip";
    mime_to_ext["/gzip"] = ".gz";
    mime_to_ext["/x-tar"] = ".tar";
    mime_to_ext["/vnd.openxmlformats-officedocument.wordprocessingml.document"] = ".docx";
    mime_to_ext["/x-c"] = ".c";
    mime_to_ext["/x-cpp"] = ".cpp";
    mime_to_ext["/x-python"] = ".py";
    mime_to_ext["/octet-stream"] = ".bin";
    mime_to_ext["/png"] = ".png";
    mime_to_ext["/jpeg"] = ".jpg";
    mime_to_ext["/gif"] = ".gif";
    mime_to_ext["/svg+xml"] = ".svg";
    mime_to_ext["/mpeg"] = ".mp3";
    mime_to_ext["/mp4"] = ".mp4";

    if (mime_to_ext.find(content_type) != mime_to_ext.end()) {
        return mime_to_ext.at(content_type);
    }
    return ".bin";
}

static string generate_file_with_ext(const string &extension) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    // Extract seconds and microseconds
    time_t rawTime = tv.tv_sec;
    int microseconds = tv.tv_usec;
    
    // Convert to local time
    struct tm *timeInfo = localtime(&rawTime);

    // Create the formatted string
    ostringstream oss;
    oss << (timeInfo->tm_year + 1900) << "_"       // Full Year (e.g., 2025)
        << (timeInfo->tm_mon + 1) << "_"           // Month
        << timeInfo->tm_mday << "_"               // Day
        << timeInfo->tm_hour << "_"               // Hours
        << timeInfo->tm_min << "_"                // Minutes
        << timeInfo->tm_sec << "_"                // Seconds
        << microseconds <<  extension;               // Microseconds

    return oss.str();
}

string extract_new_file_name(const string &info) {
    string file_name;
    size_t pos = info.find("filename=\"");
    if (pos == string::npos) {
        DEBUG_MODE && cerr << BOLD_RED << "filename not exist in post request\n" << RESET;
        return (generate_file_with_ext(".txt"));
    }
    file_name = info.substr(pos + 10); // +11 is length("filename=\"") , info.size() - 1 to not take last "
    file_name.erase(file_name.size() - 1); 
    return (file_name);
}

fstream *HttpResponse::binary_post_case()
{
    map<string, string>	header = request->get_header();
    string extention;
    if (header.find("content-type") != header.end())
    {
        string content_type = header["content-type"];
        extention = content_type.substr(content_type.find("/"));
    }
    else
        extention = "/octet-stream";
    string generated_binary_file = generate_file_with_ext(get_file_extension(extention));
    fstream *file;
    try {
        file = new fstream((request->get_server().get_locations()[index_location].get_location_upload_store()\
        + generated_binary_file).c_str(), ios::out | ios::trunc | ios::binary);
    } catch (std::exception& e) {
        DEBUG_MODE && cerr << BOLD_RED << "new failed " << e.what() << "\n" << RESET;
        request->set_is_complete_post(true);
        request->set_is_cgi(false);
        request->set_is_cgi_complete(true);
        request->set_status_code("500");
        return (NULL);
    }
    if (!file->is_open()) {
        DEBUG_MODE && cerr << "Couldn't create the new file\n";
        request->set_is_complete_post(true);
        request->set_is_cgi_complete(true);
        request->set_is_cgi(false);
        request->set_status_code("500");
        delete file;
        file = NULL;
        return NULL;
    }
    return file;
}

void HttpResponse::post_method() {
    string body = request->get_body();
    if (body.rfind("\r") != string::npos && body.rfind("\r") == body.size() - 1)
        return ;
    DEBUG_MODE && cout << BOLD_RED << "we are in post method function \n" << RESET; // to remove
    if (request->get_is_complete_post())
        return;
    map<string, string> header = request->get_header();
    DEBUG_MODE && cout << BG_YELLOW << "===================== current body before ============================\n";
    DEBUG_MODE && cout << BOLD_YELLOW << addPrefixBeforeCRLF(body) << "\n" << RESET; // to remove
    DEBUG_MODE && cout << "===============================================================\n" << RESET;

    if (request->get_is_chunked())
    {
        if (!normalize_chunked_data(body))
            return ;
    }
    fstream *file = request->get_file_stream();
    string slice;
    size_t pos_crlf = body.find(CRLF);
    size_t pos_bound_begin;
    size_t pos_bound_end;
    size_t pos_info;
    string info;
    string new_file_name;
    if (pos_crlf == string::npos && !request->get_is_binary_post()) {
        if (body.size() < BUFFER_SIZE2 && request->get_is_cgi()) {
            request->set_is_complete_post(true);
        }
        if (!file)
            DEBUG_MODE && cerr << BOLD_RED << "can't write in file in post_method() func 1\n" << RESET;
        else
            *file << body;
        request->set_body(""); // clear previous body
        return ;
    }
    while (pos_crlf != string::npos || request->get_is_binary_post()) {
        pos_bound_begin = body.find(request->get_boundary_key_begin());
        if (!request->get_boundary_key_begin().empty() && pos_bound_begin != string::npos) {
            slice = body.substr(0, pos_bound_begin);
            if (file)
                *file << slice;
            body = body.substr(pos_bound_begin);
            if (!is_crlf_exist_more_than_five_times(body)) {
                // not founded means i need more shanks to complete this task
                request->set_body(body);
                return;
            }
            body = body.substr(request->get_boundary_key_begin().size());
            pos_info = body.find(CRLF);
            info = body.substr(0, pos_info);
            new_file_name = extract_new_file_name(info);
            if (file) {
                file->close();
                delete file;
                request->set_file_stream(NULL);
            }
            
            try {
                file = new fstream((request->get_server().get_locations()[index_location].get_location_upload_store()\
                 + new_file_name).c_str(), ios::out | ios::trunc | ios::binary);
            } catch (std::exception& e) {
                DEBUG_MODE && cerr << BOLD_RED << "new failed " << e.what() << "\n" << RESET;
                request->set_is_complete_post(true);
                request->set_is_cgi(false);
                request->set_is_cgi_complete(true);
                request->set_status_code("500");
                return ;
            }
            if (!file->is_open()) {
                DEBUG_MODE && cerr << "Couldn't create the new file\n";
                request->set_is_complete_post(true);
                request->set_status_code("400");
                request->set_file_path(BAD_REQUEST);
                request->set_is_cgi(false);
                request->set_is_cgi_complete(true);
                delete file;
                file = NULL;
                return;
            }
            request->set_file_stream(file);
            request->set_is_binary_post(false);
            pos_info = body.find(CRLF_2);
            if (pos_info == string::npos)
                return ;
            body = body.substr(pos_info + 4);
            request->set_body(body);
        }
        else {
            pos_bound_end = body.find(request->get_boundary_key_end());
            if (!request->get_boundary_key_end().empty() && pos_bound_end != string::npos) {
                slice = body.substr(0, pos_bound_end);
                if (!file)
                    request->set_is_complete_post(true);
                else
                    *file << slice;
                request->set_body("");
                request->set_is_complete_post(true);
                break;
            }
            else {
                if (body.size() == 2)
                    return;
                if (request->get_is_binary_post())
                {
                    if (!file)
                    {
                        file = binary_post_case();
                        body = body.substr(2);
                    }
                    if (!file)
                        return ;
                    request->set_file_stream(file);
                }
                if (request->get_is_binary_post())
                    request->set_content_length(request->get_content_length() - body.size());
                *file << body;
                request->set_body("");
                if (request->get_is_binary_post() && request->get_content_length() <= 0)
                    request->set_is_complete_post(true);
                break;
            }
        }
        pos_crlf = body.find(CRLF);
    }
}
