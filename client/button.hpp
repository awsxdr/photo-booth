#pragma once

#include "input.hpp"

#include <memory>

namespace photo_booth {
    class Button {
    private:
        const ButtonType button_type;
        const std::shared_ptr<Input> input;

        bool is_pressed;
        bool state_change_last_frame;
    public:
        Button(ButtonType button_type, std::shared_ptr<Input> input);
        virtual ~Button();

        void tick();
        bool get_is_pressed();
        bool get_state_change_last_frame();

        bool pressed_last_frame();
        bool released_last_frame();
    };
}