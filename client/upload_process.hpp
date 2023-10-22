#pragma once

#include <string>

namespace photo_booth {
    class UploadProcess {
    private:
        static bool upload_file(std::string file_path);
        static void process_files();

    public:
        static void start();
    };
}
