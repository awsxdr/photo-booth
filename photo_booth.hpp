#pragma once

#include "button.hpp"
#include "input.hpp"
#include "window_manager.hpp"

#include <chrono>
#include <memory>

namespace photo_booth {
    class PhotoBooth {
    private:
        enum class State {
            Startup,
            Idle,
            CountingDown,
            TakingPicture,
            Previewing,
            Restarting,
            Exiting,
        };

        const std::shared_ptr<Input> input;
        const std::unique_ptr<WindowManager> window_manager;
        const std::unique_ptr<Button> green_button;
        const std::unique_ptr<Button> red_button;

        int video_process_id;
        State state;
        std::chrono::system_clock::time_point state_change_time;
        std::chrono::system_clock::time_point last_input_tick_time;
        std::chrono::system_clock::time_point last_render_time;

        void start_video_capture();
        void start_still_capture();
        void start_photo_preview();

        void input_tick();
        void idle_tick();
        void count_down_tick();
        void picture_tick();
        void preview_tick();
        void restart_tick();

        void set_state(State state);
        std::chrono::milliseconds get_milliseconds_since_state_change();

    public:
        PhotoBooth();
        virtual ~PhotoBooth();

        void run();
    };
}