#include "HttpResponse.hpp"
#include <cstddef>
#include <fstream>
#include <i386/endian.h>



// first thing search for CRLF
// if no CRLF that means i think the body should be added to the file
/*
    if CRLF exist in body that means two cases:
        case 1> new file should added
        case 2> the end of post request
    
    if CRLF exist in body but :
        if no one of this exist in body : {boundary_key_begin, boundary_key_end, Content-Disposition}
        in this case we should wait for the next shank
    
    if CRLFCRLF exist in body that means before it should exist Content-Type
*/


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


// static string addPrefixBeforeCRLF(const string &input) {
//     const string word = "\r\n";
//     const string prefix = "{$$$$}";
//     string result = input;
//     size_t pos = 0;

//     while ((pos = result.find(word, pos)) != string::npos) {
//         result.insert(pos, prefix);
//         pos += prefix.size() + word.size(); // Move past the added prefix and word
//     }

//     return result;
// }


void HttpResponse::post_method() const {
    if (request->get_is_complete_post()) return;
    string body = request->get_body(); // get body from request
    cout << BOLD_RED << "we are in post method function" << "\n"; // to remove

    // cout << "===================== current body ============================\n";
    // cout << addPrefixBeforeCRLF(body) << "\n";
    // cout << "===============================================================\n";


    fstream *file = request->get_file_stream(); // get file from request
    string slice;
    size_t pos_crlf = body.find(CRLF);
    size_t pos_bound_begin;
    size_t pos_bound_end;
    size_t pos_info;
    string info;
    string new_file_name;
    
    // CRLF not exist in body so I can safely add body to file
    if (pos_crlf == string::npos) {
        cout << "writing in file 1\n";
        // cout << BOLD_BLUE << body << "\n" << RESET;
        if (body.size() < BUFFER_SIZE2 && request->get_is_cgi()) {
            cout << body << "\n";
            request->set_is_complete_post(true);
            cerr << BOLD_RED << "post ended here\n" << RESET;
        }
        if (!file)
            cerr << BOLD_RED << "can't write in file in post_method() func 1\n" << RESET;
        else
            *file << body;

        if (file->is_open()) {
            cout << BOLD_GREEN << "is open\n" << RESET;
        }
        // file->flush();
        // std::ifstream file(request->get_cgi_path_post().c_str());
        // streampos offset = 0;
        // file.seekg(offset, ios::beg);
        // if (!file) {
        //     cerr << "Error: Unable to open file " << request->get_cgi_path_post() << '\n';
        // }
        // cout << request->get_cgi_path_post() << "\n";
        // cout << BG_CYAN<< "content file is : >> " <<file.rdbuf() << "\n" << RESET; // Write the file content to stdout

        // file.close();
        request->set_body(""); // clear previous body
        return ;
    }

    // while CRLF in the body
    // lets search for new boundary_key_begin
    // search for information of new file
    // search for boundary_key_end
    
    while (pos_crlf != string::npos) {
        // cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$ in loop \n";
        pos_bound_begin = body.find(request->get_boundary_key_begin());
        if (pos_bound_begin != string::npos) {
            // cout << "-----------------------> found boundary begin key\n";
            // every thing befor this pos_bound_begin should be added in the file
            slice = body.substr(0, pos_bound_begin);
            // cout << "=================slice ==========================\n";
            // cout << addPrefixBeforeCRLF(slice) << "\n";
            // cout << "=================================================\n"; 
            if (file) {
                *file << slice;
                cout << "writing in file 2\n";
                if (slice.find(CRLF) != string::npos) {
                    cout << "found crlf in slice\n";
                }
            }
            body = body.substr(pos_bound_begin);
            // cout << "==================== body becomes 1 ================\n";
            // cout << addPrefixBeforeCRLF(body) << "\n";
            // cout << "====================================================\n";
            // now I should find five CRLF 
            if (!is_crlf_exist_more_than_five_times(body)) {
                // not founded means i need more shanks to complete this task
                request->set_body(body); // update body
                cout << "tesssst\n";
                return;
            }
            // erase boundary_key_begin
            body = body.substr(request->get_boundary_key_begin().size());
            // cout << "==================== body becomes 2 ================\n";
            // cout << addPrefixBeforeCRLF(body) << "\n";
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
            // !!!!!!!!!!!!!!! we need to check if file name is empty
            file = new fstream((POST_PATH + new_file_name).c_str(), ios::out | ios::trunc | ios::binary);
            if (!file->is_open()) {
                cerr << "Couldn't create the new file\n";
                perror("why?");
                request->set_is_complete_post(true); // set internal server error
                cout << BOLD_GREEN << "we read all 2\n\n";
                delete file;
                file = NULL;
                return;
            }

            request->set_file_stream(file); // update file stream
            // now just move Content-Type + CRLF CRLF
            pos_info = body.find(CRLF_2);
            if (pos_info == string::npos) {
                cerr << BOLD_RED << "something wrong we couldn't find Content-Type\n" << RESET;
                return ;
            }
            // cout << "######here\n";
            body = body.substr(pos_info + 4);
            // cout << "==================== body becomes 3 ================\n";
            // cout << addPrefixBeforeCRLF(body) << "\n";
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
                    request->set_is_complete_post(true); // set internal server error
                    cout << BOLD_GREEN << "we read all 3\n\n";
                }
                else {
                    cout << "writing in file 3\n";
                    *file << slice;
                }

                request->set_body("");
                request->set_is_complete_post(true); // this task is done
                // cout << BOLD_GREEN << "we read all 4\n\n";
                break;
            } else {
                if (body.find(CRLF) != string::npos)
                    cout << "tiri tiri tiri\n";
                // cout << "==================== last slice becomes 4 ================\n";
                // cout << addPrefixBeforeCRLF(slice) << "\n";
                // cout << "====================================================\n";

                *file << body;
                request->set_body("");
                // cout << "Crlf exist but ....!\n";
                break;
            }
        }
        pos_crlf = body.find(CRLF);
    }
}
