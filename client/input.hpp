#pragma once

namespace photo_booth {
    enum class ButtonType {
        Red,
        Green
    };

    class Input {
    private:
        bool green_light_on = false;
        bool red_light_on = false;

    public:
        Input();
        virtual ~Input();

        void run_test();

        void set_light(ButtonType button, bool on);
        bool get_button_pressed(ButtonType button);
    };
}