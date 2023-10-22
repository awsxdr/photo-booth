#pragma once

#include "countdown_window.hpp"
#include "preview_window.hpp"

#include <memory>
#include <string>
#include <X11/Xlib.h>

namespace photo_booth {
    typedef struct {
        int width;
        int height;
    } Size;

    class WindowManager {
    private:
        Window top_bar;
        Window bottom_bar;
        Window flash_window;
        Window blank_window;
        Window preview_window;

        std::unique_ptr<CountdownWindow> top_countdown;
        std::unique_ptr<CountdownWindow> bottom_countdown;
        std::unique_ptr<PreviewWindow> preview;

        std::shared_ptr<Display> display;
        const int screen;

        const Size display_size;

        bool is_preview_visible;

        Window create_borderless_window(int x, int y, int width, int height, int depth, Visual* visual, unsigned long background_color) const;
        void set_always_on_top(const Window& window) const;

    public:
        WindowManager();
        virtual ~WindowManager();

        bool events_tick() const;
        void render() const;

        void show_flash() const;
        void hide_flash() const;

        void show_blank() const;
        void hide_blank() const;

        void show_preview(std::string path);
        void hide_preview();

        void set_countdown_state(CountdownState state);
    };
}