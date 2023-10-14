#pragma once

#include <GL/gl.h>
#include <string>

namespace photo_booth {
    class TextureLoader {
    private:
        GLuint texture_from_data(unsigned char* image_data, int width, int height, GLint format);

    public:
        TextureLoader();
        virtual ~TextureLoader();

        GLuint load_png(std::string path);
        GLuint load_jpeg(std::string path);

        class InvalidFileFormatException : public std::exception { };
        class OutOfMemoryException : public std::exception { };
        class ImageReadException : public std::exception { };
    };
}
