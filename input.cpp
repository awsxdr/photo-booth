#include "input.hpp"

#include <chrono>
#include <pigpio.h>
#include <thread>

using namespace std;
using namespace photo_booth;

const int PIN_GREEN_OUTPUT = 19;
const int PIN_GREEN_INPUT = 26;
const int PIN_RED_OUTPUT = 16;
const int PIN_RED_INPUT = 20;
const int PIN_GREEN_LIGHT = 21;
const int PIN_RED_LIGHT = 13;

Input::Input() {
    gpioInitialise();

    gpioSetMode(PIN_GREEN_INPUT, PI_INPUT);
    gpioSetMode(PIN_GREEN_OUTPUT, PI_OUTPUT);

    gpioSetMode(PIN_RED_INPUT, PI_INPUT);
    gpioSetMode(PIN_RED_OUTPUT, PI_OUTPUT);

    gpioSetMode(PIN_GREEN_LIGHT, PI_OUTPUT);
    gpioSetMode(PIN_RED_LIGHT, PI_OUTPUT);

    gpioWrite(PIN_GREEN_OUTPUT, PI_HIGH);
    gpioWrite(PIN_RED_OUTPUT, PI_HIGH);
    gpioWrite(PIN_GREEN_LIGHT, PI_LOW);
    gpioWrite(PIN_RED_LIGHT, PI_LOW);
}

Input::~Input() {
    gpioWrite(PIN_GREEN_OUTPUT, PI_LOW);
    gpioWrite(PIN_RED_OUTPUT, PI_LOW);
    gpioWrite(PIN_GREEN_LIGHT, PI_LOW);
    gpioWrite(PIN_RED_LIGHT, PI_LOW);

    gpioTerminate();
}

void Input::run_test() {
    for(auto i = 0; i < 5; ++i) {
        set_light(Button::Green, true);
        set_light(Button::Red, true);
        this_thread::sleep_for(chrono::milliseconds(500));

        set_light(Button::Green, false);
        set_light(Button::Red, false);
        this_thread::sleep_for(chrono::milliseconds(500));
    }
}

void Input::set_light(Button button, bool value) {
    auto const set_light = [value](int light_pin) {
        gpioWrite(light_pin, value ? PI_HIGH : PI_LOW);
    };

    switch(button) {
        case Button::Green:
            this->green_light_on = value;
            set_light(PIN_GREEN_LIGHT);
            break;

        case Button::Red:
            this->red_light_on = value;
            set_light(PIN_RED_LIGHT);
            break;
    }
}

bool Input::get_button_pressed(Button button) {
    switch(button) {
        case Button::Green:
            return gpioRead(PIN_GREEN_INPUT) != 0;

        case Button::Red:
            return gpioRead(PIN_RED_INPUT) != 0;
    }

    return false;
}