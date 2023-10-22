#pragma once

#include <string>

namespace photo_booth {
    class UploadProcess {
    private:
        static bool upload_file(std::string file_path);
        static void process_files();
        static std::string get_signed_url(std::string file_path);

    public:
        static void start();
    };
}
