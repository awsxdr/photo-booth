#include "button.hpp"

#include <iostream>

using namespace photo_booth;
using namespace std;

Button::Button(ButtonType button_type, std::shared_ptr<Input> input) :
    button_type(button_type),
    input(input),
    is_pressed(false),
    state_change_last_frame(false)
{
}

Button::~Button() {
}

void Button::tick() {
    auto const is_pressed = this->input->get_button_pressed(this->button_type);

    this->state_change_last_frame = is_pressed != this->is_pressed;
    this->is_pressed = is_pressed;
}

bool Button::get_is_pressed() {
    return this->is_pressed;
}

bool Button::get_state_change_last_frame() {
    return this->state_change_last_frame;
}

bool Button::pressed_last_frame() {
    return this->is_pressed && this->state_change_last_frame;
}

bool Button::released_last_frame() {
    return !this->is_pressed && this->state_change_last_frame;
}