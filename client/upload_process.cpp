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
    auto const curl_command = "curl -F \"image=@" + file_path + "\" https://tlwedding.awsxdr.com/api/photos";
    auto const pipe = popen(curl_command.c_str(), "r");

    auto const return_code = pclose(pipe);

    return return_code == EXIT_SUCCESS;
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