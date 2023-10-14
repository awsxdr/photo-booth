#pragma once

#include "texture_loader.hpp"

#include <chrono>
#include <GL/gl.h>
#include <GL/glx.h>
#include <map>
#include <memory>
#include <string>
#include <X11/Xlib.h>

namespace photo_booth {
    enum class CountdownState {
        Starting,
        Idle,
        CountingDown,
        PhotoVerify
    };

    class CountdownWindow {
    private:
        enum class TextureType {
            Number1,
            Number2,
            Number3,
            Number4,
            Number5,
            Start,
            GetReady,
            Review
        };

        const Window window;
        const std::shared_ptr<Display> display;
        const std::unique_ptr<TextureLoader> texture_loader;

        std::chrono::system_clock::time_point state_update_time;
        CountdownState state;
        XVisualInfo* visual_info;
        GLXContext context;

        std::map<TextureType, GLuint> textures;

        void load_textures();

        void render_starting();
        void render_idle();
        void render_countdown();
        void render_verify();

    public:
        CountdownWindow(std::shared_ptr<Display> display, Window window, XVisualInfo* visual_info);
        virtual ~CountdownWindow();

        void render();
        void set_state(CountdownState state);
    };
}