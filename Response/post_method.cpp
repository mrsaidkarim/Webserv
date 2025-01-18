/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   post_method.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skarim <skarim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/15 12:54:20 by skarim            #+#    #+#             */
/*   Updated: 2025/01/16 23:05:51 by skarim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"


/*
    2 crlf for boundary_key_begin
    3 crlf for new file information
*/
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

void normalize_chunked_data(string &s) {
    const string crlf = "\r\n";
    string res; // To store the processed data
    size_t pos = 0;

    while ((pos = s.find(crlf, pos)) != string::npos) {
        size_t chunk_start = pos + crlf.size();
        size_t i = chunk_start;

        while (i < s.size() && isxdigit(s[i])) {
            i++;
        }

        if (i > chunk_start && i + 1 < s.size() && s[i] == '\r' && s[i + 1] == '\n') {
            s = s.substr(0, pos) + s.substr(i + crlf.size());
        } else {
            pos += crlf.size();
        }
    }
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

static string get_file_extension(const string& content_type) {
    const map<string, string> mime_to_ext = {
        {"/html", ".html"},
        {"/plain", ".txt"},
        {"/css", ".css"},
        {"/javascript", ".js"},
        {"/json", ".json"},
        {"/xml", ".xml"},
        {"/pdf", ".pdf"},
        {"/zip", ".zip"},
        {"/gzip", ".gz"},
        {"/x-tar", ".tar"},
        {"/vnd.openxmlformats-officedocument.wordprocessingml.document", ".docx"},
        {"/x-c", ".c"},
        {"/x-cpp", ".cpp"},
        {"/x-python", ".py"},
        {"/octet-stream", ".bin"},
        {"/png", ".png"},
        {"/jpeg", ".jpg"},
        {"/gif", ".gif"},
        {"/svg+xml", ".svg"},
        {"/mpeg", ".mp3"},
        {"/mp4", ".mp4"},
    };

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
        cerr << BOLD_RED << "filename not exist in post request\n" << RESET;
        return (generate_file_with_ext(".txt")); // i just return this for ... debug reasone
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
    // cout << BOLD_MAGENTA << "content-type:" << content_type << " ==> the extension: " << get_file_extension(extention) << endl << RESET;
    // fstream *file = new fstream((POST_PATH + generated_binary_file).c_str(), ios::out | ios::trunc | ios::binary);
    fstream *file = new fstream((request->get_server().get_locations()[index_location].get_location_upload_store()\
     + generated_binary_file).c_str(), ios::out | ios::trunc | ios::binary);
    if (!file->is_open()) {
        cerr << "Couldn't create the new file\n";
        perror("why?");
        request->set_is_complete_post(true);
        cerr << BOLD_RED << "###########   : 1\n" << RESET;
        cout << BOLD_GREEN << "we read all 2\n\n";
        delete file;
        file = NULL;
        return NULL;
    }
    return file;
}

// static bool check(map<string, string> header)
// {
//     if (header.find("contnet-type") != header.end())
//     {
//         if(header["content-type"] != "application/x-www-form-urlencoded")
//             return true;
//     }
//     return false;
// }
void HttpResponse::post_method() {
    string body = request->get_body();
    // cout << BOLD_RED << "we are in post method function \n" << RESET; // to remove
    if (request->get_is_complete_post()) return;
    // if (body.empty())
    // {
    //     request->set_is_complete(true);
    //     return ;
    // }
    auto header = request->get_header();
    // cout << BOLD_RED;
    // for(auto element = header.begin(); element != header.end(); element++)
    //     cout << element->first << ": " << element->second << "\n";
    // cout << RESET;
    // cout << BG_YELLOW << "===================== current body before ============================\n";
    // cout << BOLD_YELLOW << addPrefixBeforeCRLF(body) << "\n" << RESET; // to remove
    // cout << "===============================================================\n" << RESET;
    if (request->get_is_chunked() || request->get_is_complete())
        normalize_chunked_data(body);
    // cout << BG_BLUE << "===================== current body after ============================\n";
    // cout << addPrefixBeforeCRLF(body) << "\n"; // to remove
    // cout << "===============================================================\n" << RESET;
    fstream *file = request->get_file_stream(); // get file from request
    string slice;
    size_t pos_crlf = body.find(CRLF);
    size_t pos_bound_begin;
    size_t pos_bound_end;
    size_t pos_info;
    string info;
    string new_file_name;
    // cout << BOLD_GREEN << "*************: <<<" <<request->get_is_binary_post() <<">>>>" << request->get_content_length() << endl << RESET;
    
    // if (request->get_is_complete ()) {
    //     cout << BG_GREEN << "completed here\n" << RESET;
    //     // cout << body << "\n" << RESET;
    // }
    if (pos_crlf == string::npos && !request->get_is_binary_post()) { //
        // cout << "dkhal hneyaaaaaaaaaa\n";
        if (body.size() < BUFFER_SIZE2 && request->get_is_cgi()) {
            // cout << body << "\n";
            request->set_is_complete_post(true);
            // cerr << BOLD_RED << "###########   : 2\n" << RESET;
            // cerr << BOLD_RED << "post ended here\n" << RESET;
        }
        if (!file)
            cerr << BOLD_RED << "can't write in file in post_method() func 1\n" << RESET;
        else {
            *file << body;
            // cout << "hereeeee\n";
        }
        request->set_body(""); // clear previous body
        // cout << BG_BLUE << "reh treseta lbody\n" << RESET;
        return ;
    }
    // cout << BG_BLUE << "ymken makynash crlf" << RESET;
    while (pos_crlf != string::npos || request->get_is_binary_post()) {
        // cout << BG_CYAN << "keyn am3alem crlf<<"  << "\n" << RESET;
        pos_bound_begin = body.find(request->get_boundary_key_begin());
        if (!request->get_boundary_key_begin().empty() && pos_bound_begin != string::npos) {
            // cout << BG_GREEN << "keyn boundary_key_begin" << RESET;
            // every thing befor this pos_bound_begin should be added in the file
            slice = body.substr(0, pos_bound_begin);
            if (file) {
                *file << addPrefixBeforeCRLF(slice);
                // cout << "here\n";
            }
            body = body.substr(pos_bound_begin);
            // now I should find five CRLF 
            if (!is_crlf_exist_more_than_five_times(body)) {
                // not founded means i need more shanks to complete this task
                request->set_body(body); // update body
                return;
            }
            // erase boundary_key_begin
            body = body.substr(request->get_boundary_key_begin().size());
            // search for CRLF after Content-Disposition
            pos_info = body.find(CRLF);
            info = body.substr(0, pos_info);
            // get name of new file
            new_file_name = extract_new_file_name(info);
            if (file) {
                file->close();
                delete file;
                request->set_file_stream(NULL);
            }
            // fill new information;
            // file = new fstream((POST_PATH + new_file_name).c_str(), ios::out | ios::trunc | ios::binary);
            file = new fstream((request->get_server().get_locations()[index_location].get_location_upload_store()\
             + new_file_name).c_str(), ios::out | ios::trunc | ios::binary);
            if (!file->is_open()) {
                // cerr << "Couldn't create the new file\n";
                perror("why?");
                request->set_is_complete_post(true);
                request->set_file_path(BAD_REQUEST);
                // cerr << BOLD_RED << "###########   : 3\n" << RESET;
                // cout << BOLD_GREEN << "we read all 2\n\n";
                delete file;
                file = NULL;
                return;
            }
            request->set_file_stream(file);
            request->set_is_binary_post(false); // update file stream
            // cout << BG_GREEN << "rja3 false    \n" << RESET;
            // now just move Content-Type + CRLF CRLF
            pos_info = body.find(CRLF_2);
            if (pos_info == string::npos) {
                // cerr << BOLD_RED << "something wrong we couldn't find Content-Type\n" << RESET;
                return ;
            }
            body = body.substr(pos_info + 4);
            // cout << "==================== body becomes 3 ================\n";
            // cout << body << "\n";
            // cout << "====================================================\n";
            request->set_body(body);
        }
        else {
            // cout << BG_YELLOW << "i9der ikon boundary_key_end" << RESET;
            pos_bound_end = body.find(request->get_boundary_key_end());
            // we are done
            if (!request->get_boundary_key_end().empty() && pos_bound_end != string::npos) {
                // cout << "---------------> found boundary key end \n";
                slice = body.substr(0, pos_bound_end);
                if (!file) {
                    // cerr << BOLD_RED << "can't write in file in post_method() func 2\n" << RESET;
                    request->set_is_complete_post(true);
                    // cerr << BOLD_RED << "###########   : 4\n" << RESET;
                    // cout << BOLD_GREEN << "we read all 3\n\n";
                }
                else {
                    *file << slice;
                    // file->flush();
                    // cout << "here\n";
                }

                request->set_body("");
                request->set_is_complete_post(true);
                // cerr << BOLD_RED << "###########   : 5\n" << RESET; // this task is done
                // cout << BOLD_GREEN << "we read all 4\n\n";
                break;
            // } else {
            //     *file << body;
            //     request->set_body("");
            //     break;
            // }
            }
            else {
                // if (body.rfind("\r") != string::npos && last != string::npos)
                //     last = (last > body.rfind("\r")) ? last : body.rfind("\r");
                // // if (last == string::npos && !body.empty()) {
                //     *file << addPrefixBeforeCRLF(body); // Write remaining body
                //     request->set_body("");
                // }
                
                if (request->get_is_binary_post())
                {
                    if (!file)
                    {
                        file = binary_post_case();
                        body = body.substr(2);
                        // last += 2;
                    }
                    if (!file)
                        return ;
                    request->set_file_stream(file);
                }
                size_t last = body.rfind(CRLF);
                // if (last != string::npos && body.length() - last < 500)
                // if (last != string::npos && body.length() - last < 500 && check(header)) // protect content-type
                if (last != string::npos && body.length() - last < 500 && header["content-type"] != "application/x-www-form-urlencoded")
                {
                    // cout << BOLD_RED << "*************: " << addPrefixBeforeCRLF(body.substr(0, last)) << endl << RESET;//
                    // cout << BOLD_GREEN << "lbody li t7at: " <<  addPrefixBeforeCRLF(body.substr(0, last)) << endl << RESET;
                    *file << body.substr(0, last);
                    request->set_body(body.substr(last));
                    // file->flush(); // just for test
                    if (request->get_is_binary_post())
                        request->set_content_length(request->get_content_length() - last);
                        // request->content_length -= last;
                    // cout << BOLD_RED << "*************: " << request->get_content_length() << "|||||" << body.size()<< endl << RESET;
                    // cout << BOLD_YELLOW << "lbody li b9a: " << addPrefixBeforeCRLF(body.substr(last)) << endl << RESET;
                }
                else
                {
                    // cout << BOLD_GREEN << "*************: <<<" <<request->get_is_binary_post() <<">>>>" << request->get_content_length() << endl << RESET;
                    if (request->get_is_binary_post())
                        request->set_content_length(request->get_content_length() - body.size());
                    // cout << BOLD_RED << "*************: " << request->get_content_length() << "$$$$$$" << body.size()<< endl << RESET;
                    // else
                    // {
                        if (header["content-type"] != "application/x-www-form-urlencoded")
                            body = body.substr(0, request->get_content_length());
                        *file << body;
                        // cout << BOLD_RED << "*************:>>>" << addPrefixBeforeCRLF(body) << "<<<" << endl << RESET;
                        // file->flush();
                        request->set_body("");
                    // }
                }
                if (request->get_is_binary_post() && request->get_content_length() <= 0)
                {
                    // cout << BOLD_CYAN<< "*************: " << body << endl << RESET; 
                    request->set_is_complete_post(true);
                    // cerr << BOLD_RED << "###########   : 6\n" << RESET;
                    
                }
                // {
                    // request->set_body(""); //not necessary just for some tests
                    // cout << "set_is_complete(true)\n";
                    // break ;
                //     return ;
                // }
                // cout << BOLD_CYAN << "hedshi thal flfile: " << addPrefixBeforeCRLF(body.substr(0, pos_crlf)) << endl << RESET;
                // body = body.substr(pos_crlf + 2);
                
                // cout << BOLD_RED << "hedshi wela how lbody: " << addPrefixBeforeCRLF(request->get_body()) << endl << RESET;
                break;
            }
            cout << "ba9in fi loop\n";
        }
        pos_crlf = body.find(CRLF);
    }
    // cout << BOLD_BLUE << "ghadi ikhroj men post\n" << RESET;
}
