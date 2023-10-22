#include "upload_process.hpp"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>

using namespace photo_booth;
using namespace std;

void UploadProcess::start() {
    cout << "Forking for uploads process" << endl;
    
    auto const process_id = fork();

    if(process_id == 0) {
        process_files();
        exit(EXIT_SUCCESS);
    }
}

bool UploadProcess::upload_file(string file_path) {
    auto const signed_url = get_signed_url(file_path);

    if(signed_url == "") {
        return false;
    }

    auto const curl_command = "curl -X PUT -T " + file_path + " \"" + signed_url + "\"";
    auto const pipe = popen(curl_command.c_str(), "r");

    auto const return_code = pclose(pipe);

    return return_code == EXIT_SUCCESS;
}

string UploadProcess::get_signed_url(string file_path) {
    auto const curl_command = "curl -X POST -H \"Content-Type: application/json\" -d '{\"fileName\": \"" + filesystem::path(file_path).filename().string() + "\"}' https://tlwedding.awsxdr.com/api/photos";
    auto const pipe = popen(curl_command.c_str(), "r");

    char buffer[128];
    string signed_url_json = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != nullptr) {
            signed_url_json += buffer;
        }
    }

    cout << "!!!!!!!!!!! Signed URL JSON: " << signed_url_json << endl;

    if(signed_url_json.length() < 16) {
        cout << "Invalid signed URL returned from API" << endl;
        return "";
    }

    auto const return_code = pclose(pipe);

    if(return_code != EXIT_SUCCESS) {
        return "";
    }

    return signed_url_json.substr(14, signed_url_json.length() - 16);
}

void UploadProcess::process_files() {
    cout << "Starting upload processing" << endl;

    if(!filesystem::exists("./uploads/")) {
        cout << "Creating uploads folder" << endl;
        filesystem::create_directory("./uploads/");
    }

    while(true) {
        
        auto const files = filesystem::directory_iterator("./uploads/");

        for(auto const& file: files) {
            if(upload_file(file.path())) {
                filesystem::remove(file.path());
            } else {
                cout << "Failed to upload file '" << file.path() << "'" << endl;
                // Assume a connection problem - back off and wait for it to resolve
                this_thread::sleep_for(chrono::seconds(10));
            }
        }
    }
}