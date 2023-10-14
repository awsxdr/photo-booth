#include "texture_loader.hpp"

#include <GL/gl.h>
#include <GL/glx.h>
#include <iostream>
#include <jpeglib.h>
#include <memory>
#include <png.h>
#include <string>

using namespace photo_booth;
using namespace std;

TextureLoader::TextureLoader() {
}

TextureLoader::~TextureLoader() {
}

GLuint TextureLoader::load_png(string path) {
    auto file_stream = fopen(path.c_str(), "rb");
    
    char signature[8];
    fread(signature, 1, 8, file_stream);

    if(!png_check_sig(reinterpret_cast<unsigned char*>(signature), 8)) {
        throw TextureLoader::InvalidFileFormatException();
    }

    auto png_data = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if(!png_data) {
        throw TextureLoader::OutOfMemoryException();
    }

    auto png_info = png_create_info_struct(png_data);
    if(!png_info) {
        throw TextureLoader::OutOfMemoryException();
    }

    if (setjmp(png_jmpbuf(png_data))) {
        png_destroy_read_struct(&png_data, &png_info, nullptr);
        throw TextureLoader::ImageReadException();
    }

    png_init_io(png_data, file_stream);
    png_set_sig_bytes(png_data, 8);
    png_read_info(png_data, png_info);
    unsigned int width, height;
    int bit_depth, color_type;
    png_get_IHDR(png_data, png_info, &width, &height, &bit_depth, &color_type, nullptr, nullptr, nullptr);

    png_read_update_info(png_data, png_info);
    auto const row_byte_count = png_get_rowbytes(png_data, png_info);
    auto const channel_count = png_get_channels(png_data, png_info);

    auto image_data = make_unique<unsigned char[]>(row_byte_count * height);
    auto row_pointers = make_unique<unsigned char*[]>(height);

    for(auto row = 0; row < height; ++row) {
        row_pointers.get()[row] = image_data.get() + row * row_byte_count;
    }

    png_read_image(png_data, row_pointers.get());
    png_read_end(png_data, nullptr);

    fclose(file_stream);

    auto const texture = this->texture_from_data(image_data.get(), width, height, GL_RGBA);

    cout << "Loaded image \"" << path << "\" (" << width << " x " << height << ") -> " << texture << endl;

    return texture;
}

GLuint TextureLoader::load_jpeg(string path) {
    auto file_stream = fopen(path.c_str(), "rb");

    jpeg_decompress_struct jpeg_info;
    jpeg_error_mgr error_manager;
    jpeg_info.err = jpeg_std_error(&error_manager);

    jpeg_create_decompress(&jpeg_info);
    jpeg_stdio_src(&jpeg_info, file_stream);
    jpeg_read_header(&jpeg_info, TRUE);

    jpeg_start_decompress(&jpeg_info);
    auto const width = jpeg_info.output_width;
    auto const height = jpeg_info.output_height;
    auto const component_count = jpeg_info.num_components;

    auto image_data = make_unique<unsigned char[]>(width * height * 3);
    auto row_pointers = make_unique<unsigned char*[]>(height);

    for(auto row = 0; row < height; ++row) {
        row_pointers.get()[row] = image_data.get() + row * width * 3;
        jpeg_read_scanlines(&jpeg_info, &row_pointers.get()[row], 1);
    }

    jpeg_finish_decompress(&jpeg_info);
    jpeg_destroy_decompress(&jpeg_info);
    fclose(file_stream);

    //jpeg_read_scanlines(&jpeg_info, row_pointers.get(), height);

    auto const texture = this->texture_from_data(image_data.get(), width, height, GL_RGB);

    cout << "Loaded image \"" << path << "\" (" << width << " x " << height << ") -> " << texture << endl;

    return texture;
}

GLuint TextureLoader::texture_from_data(unsigned char* image_data, int width, int height, GLint format) {
    GLuint texture;

    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image_data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}