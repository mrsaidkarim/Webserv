/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   post_method.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skarim <skarim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/15 12:54:20 by skarim            #+#    #+#             */
/*   Updated: 2024/12/25 15:27:19 by skarim           ###   ########.fr       */
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

// void normalize_chunked_data(string &s)
// {
//     string res; // contain the data without the chunk_size_pattern
//     size_t pos = 0;
//     size_t i;
//     while ((pos = s.find(CRLF)) != string::npos)
//     {
//         i = pos + 2;
//         while (i < s.size() && isxdigit(s[i]))
//             i++;
//         if (i != pos + 2 && i < s.size() - 1 && s[i] == '\r' && s[i + 1] == '\n')//&s[i] == CRLF)
//         {
//             s = s.substr(0, pos) + s.substr(i + 2, s.size());
//             cout << BG_RED << "*****hereeeeeeeeeee " << s << "\n" << RESET;
//         }
//         else
//             pos += 2;
//     }
// }
// void normalize_chunked_data(string &s) {
//     size_t pos = 0;

//     while ((pos = s.find(CRLF, pos)) != string::npos) {
//         size_t chunk_start = pos + 2;
//         size_t i = chunk_start;

//         while (i < s.size() && isxdigit(s[i])) {
//             i++;
//         }

//         if (i > chunk_start && i + 1 < s.size() && s[i] == '\r' && s[i + 1] == '\n') {
//             s = s.substr(0, pos) + s.substr(i + 2);
//             pos = pos;
//         } else {
//             pos += 2;
//         }
//     }
//     cout << BOLD_BLUE << pos << "normalized\n" << RESET;
// }
void normalize_chunked_data(string &s) {
    const string crlf = "\r\n";
    string res; // To store the processed data
    size_t pos = 0;

    while ((pos = s.find(crlf, pos)) != string::npos) {
        size_t chunk_start = pos + crlf.size(); // Start of potential chunk size
        size_t i = chunk_start;

        // Check if the chunk size is a valid hexadecimal number
        while (i < s.size() && isxdigit(s[i])) {
            i++;
        }

        // Ensure the detected chunk size is followed by crlf
        if (i > chunk_start && i + 1 < s.size() && s[i] == '\r' && s[i + 1] == '\n') {
            // Remove the chunk size pattern (chunk size + crlf)
            s = s.substr(0, pos) + s.substr(i + crlf.size());
            pos = pos; // Reset position for further processing
        } else {
            pos += crlf.size(); // Move past CRLF if no valid chunk size is found
        }
    }
    // cout << BOLD_BLUE << pos << "normalized\n" << RESET;
}



string extract_new_file_name(const string &info) {
    string file_name;
    size_t pos = info.find("filename=\"");
    if (pos == string::npos) {
        cerr << BOLD_RED << "filename not exist in post request\n" << RESET;
        return ("tmp.error"); // i just return this for ... debug reasone
    }
    file_name = info.substr(pos + 10); // +11 is length("filename=\"") , info.size() - 1 to not take last "
    file_name.erase(file_name.size() - 1); 
    return (file_name);
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

void HttpResponse::post_method() const {
    string body = request->get_body();
    // cout << BOLD_RED << "we are in post method function" << "\n"; // to remove

    // cout << BG_YELLOW << "===================== current body before ============================\n";
    // cout << addPrefixBeforeCRLF(body) << "\n"; // to remove
    // cout << "===============================================================\n" << RESET;
    if (request->get_is_chunked())
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
    
    // if (request->get_is_complete()) {
    //     cout << BG_GREEN << "completed here\n" << RESET;
    //     // cout << body << "\n" << RESET;
    // }
    if (pos_crlf == string::npos) {
        if (!file)
            cerr << BOLD_RED << "can't write in file in post_method() func 1\n" << RESET;
        else {
            *file << body;
            cout << "here\n";
        }
        request->set_body(""); // clear previous body
        // cout << BG_BLUE << "reh treseta lbody\n" << RESET;
        return ;
    }
    
    while (pos_crlf != string::npos) {
        pos_bound_begin = body.find(request->get_boundary_key_begin());
        if (pos_bound_begin != string::npos) {
            // cout << "-----------------------> found boundary begin key\n";
            // every thing befor this pos_bound_begin should be added in the file
            slice = body.substr(0, pos_bound_begin);
            // cout << "=================slice ==========================\n";
            // cout << slice << "\n";
            // cout << "=================================================\n"; 
            if (file) {
                *file << addPrefixBeforeCRLF(slice);
                cout << "here\n";
            }
            body = body.substr(pos_bound_begin);
            // cout << "==================== body becomes 1 ================\n";
            // cout << body << "\n";
            // cout << "====================================================\n";
            // now I should find five CRLF 
            if (!is_crlf_exist_more_than_five_times(body)) {
                // not founded means i need more shanks to complete this task
                request->set_body(body); // update body
                return;
            }
            // erase boundary_key_begin
            body = body.substr(request->get_boundary_key_begin().size());
            // cout << "==================== body becomes 2 ================\n";
            // cout << body << "\n";
            // cout << "====================================================\n";
            // search for CRLF after Content-Disposition
            pos_info = body.find(CRLF);
            info = body.substr(0, pos_info);
            // cout << "<<<<" << info << ">>>>>" << "\n";
            // get name of new file
            new_file_name = extract_new_file_name(info);
            // cout << "file name: <<<<" << new_file_name << ">>>>\n";
            if (file) {
                file->close();
                delete file;
                request->set_file_stream(NULL);
            }
            // fill new information;
            file = new fstream((POST_PATH + new_file_name).c_str(), ios::out | ios::trunc | ios::binary);
            if (!file->is_open()) {
                cerr << "Couldn't create the new file\n";
                perror("why?");
                request->set_is_complete(true);
                cout << BOLD_GREEN << "we read all 2\n\n";
                delete file;
                file = NULL;
                return;
            }
            // cout << BG_MAGENTA << file << (POST_PATH + new_file_name).c_str() << "\n" << RESET;
            request->set_file_stream(file); // update file stream
            // now just move Content-Type + CRLF CRLF
            pos_info = body.find(CRLF_2);
            if (pos_info == string::npos) {
                cerr << BOLD_RED << "something wrong we couldn't find Content-Type\n" << RESET;
                return ;
            }
            body = body.substr(pos_info + 4);
            // cout << "==================== body becomes 3 ================\n";
            // cout << body << "\n";
            // cout << "====================================================\n";
            request->set_body(body);
        }
        else {
            pos_bound_end = body.find(request->get_boundary_key_end());
            // we are done
            if (pos_bound_end != string::npos) {
                // cout << "---------------> found boundary key end \n";
                slice = body.substr(0, pos_bound_end);
                if (!file) {
                    cerr << BOLD_RED << "can't write in file in post_method() func 2\n" << RESET;
                    request->set_is_complete(true);
                    cout << BOLD_GREEN << "we read all 3\n\n";
                }
                else {
                    *file << addPrefixBeforeCRLF(slice);
                    cout << "here\n";
                }

                request->set_body("");
                request->set_is_complete(true); // this task is done
                cout << BOLD_GREEN << "we read all 4\n\n";
                break;
            // } else {
            //     *file << body;
            //     request->set_body("");
            //     break;
            // }
            }
            else {
                size_t last = body.rfind(CRLF);
                // if (body.rfind("\r") != string::npos && last != string::npos)
                //     last = (last > body.rfind("\r")) ? last : body.rfind("\r");
                // // if (last == string::npos && !body.empty()) {
                //     *file << addPrefixBeforeCRLF(body); // Write remaining body
                //     request->set_body("");
                // }
                if (last != string::npos && body.length() - last < 500)
                {
                    // cout << BOLD_GREEN << "lbody li t7at: " <<  addPrefixBeforeCRLF(body.substr(0, last)) << endl << RESET;
                    *file << body.substr(0, last);
                    request->set_body(body.substr(last));
                    // cout << BOLD_RED << "*************: " << endl << RESET;
                    // << addPrefixBeforeCRLF(body.substr(last)) << endl << RESET;
                }
                else
                {
                    *file << body;
                    request->set_body("");
                    // cout << BG_RED << 
                }
                // cout << BOLD_CYAN << "hedshi thal flfile: " << addPrefixBeforeCRLF(body.substr(0, pos_crlf)) << endl << RESET;
                // body = body.substr(pos_crlf + 2);
                
                // cout << BOLD_RED << "hedshi wela how lbody: " << addPrefixBeforeCRLF(request->get_body()) << endl << RESET;
                break;
            }
            cout << "ba9in fi loop\n";
        }
        pos_crlf = body.find(CRLF);
    }
}