#pragma once

#include "texture_loader.hpp"

#include <GL/glx.h>
#include <string>
#include <memory>
#include <X11/Xlib.h>

namespace photo_booth {
    class PreviewWindow {
    private:
        const Window window;
        const std::shared_ptr<Display> display;
        const std::unique_ptr<TextureLoader> texture_loader;

        XVisualInfo* visual_info;
        GLXContext context;
        GLuint preview_texture;

    public:
        PreviewWindow(std::shared_ptr<Display> display, Window window, XVisualInfo* visual_info);
        virtual ~PreviewWindow();

        void render();
        void load_preview(std::string path);
    };
}