#include "input.hpp"
#include "photo_booth.hpp"
#include "window_manager.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <signal.h>
#include <stdlib.h>
#include <thread>
#include <unistd.h>

using namespace photo_booth;
using namespace std;

PhotoBooth::PhotoBooth() :
    input(make_shared<Input>()),
    window_manager(make_unique<WindowManager>()),
    state(State::Startup),
    state_change_time(chrono::system_clock::now()),
    last_input_tick_time(chrono::system_clock::now()),
    last_render_time(chrono::system_clock::now()),
    green_button(make_unique<Button>(ButtonType::Green, this->input)),
    red_button(make_unique<Button>(ButtonType::Red, this->input))
{
}

PhotoBooth::~PhotoBooth() {
    if(this->video_process_id) {
        kill(this->video_process_id, 1);
        this->video_process_id = 0;
    }
}

void PhotoBooth::run() {
    this->window_manager->show_flash();

    this->start_video_capture();

    this->input->run_test();

    this->window_manager->hide_flash();

    this->set_state(State::Idle);
    this->window_manager->set_countdown_state(CountdownState::Idle);
    this->input->set_light(ButtonType::Green, true);

    while (true) {
        if(chrono::system_clock::now() - this->last_input_tick_time > chrono::milliseconds(100)) {
            this->input_tick();
            this->last_input_tick_time = chrono::system_clock::now();
        }

        if(chrono::system_clock::now() - this->last_render_time > chrono::milliseconds(100)) {
            this->window_manager->render();
            this->last_render_time = chrono::system_clock::now();
        }

        if(!this->window_manager->events_tick()) {
            this_thread::sleep_for(chrono::milliseconds(1));
        }

        switch (state) {
        case State::Idle:
            this->idle_tick();
            break;

        case State::CountingDown:
            this->count_down_tick();
            break;

        case State::TakingPicture:
            this->picture_tick();
            break;

        case State::Previewing:
            this->preview_tick();
            break;

        case State::Restarting:
            this->restart_tick();
            break;

        case State::Exiting:
            if(this->video_process_id) {
                kill(this->video_process_id, 1);
                this->video_process_id = 0;
            }
            return;
        }
    }
}

void PhotoBooth::start_video_capture() {
    auto const process_id = fork();

    if(process_id == 0) {
        execlp("libcamera-vid", "libcamera-vid", "-f", "-t", "0", "--hflip", "--width", "1920", "--height", "1080", "--framerate", "25", "--inline", "--gain", "50", nullptr);
        exit(0);
    }

    this->video_process_id = process_id;
}

void PhotoBooth::start_still_capture() {
    auto const process_id = fork();

    if(process_id == 0) {
        execlp("libcamera-jpeg", "libcamera-jpeg", "-o", "./test.jpeg", "-n", "-t", "1", "--width", "4056", "--height", "2282", "--immediate", nullptr);
        exit(0);
    }
}

void PhotoBooth::input_tick() {
    this->green_button->tick();
    this->red_button->tick();
}

void PhotoBooth::idle_tick() {
    if(this->green_button->pressed_last_frame()) {
        cout << "Taking picture" << endl;

        this->input->set_light(ButtonType::Red, true);
        this->input_tick();

        this->set_state(State::CountingDown);
        this->window_manager->set_countdown_state(CountdownState::CountingDown);
    }

    if(this->red_button->pressed_last_frame()) {
        this->set_state(State::Exiting);
        this->window_manager->set_countdown_state(CountdownState::Starting);
    }
}

void PhotoBooth::count_down_tick() {
    auto const milliseconds_since_last_state_change = this->get_milliseconds_since_state_change();

    this->input->set_light(ButtonType::Green, ((milliseconds_since_last_state_change.count() / 500) % 2) == 0);

    if(milliseconds_since_last_state_change > chrono::seconds(5)) {
        this->set_state(State::TakingPicture);
        this->window_manager->set_countdown_state(CountdownState::PhotoVerify);
        this->input->set_light(ButtonType::Green, false);
        this->input->set_light(ButtonType::Red, false);

        this->window_manager->show_flash();

        kill(video_process_id, 1);
        this->start_still_capture();
    }

    if(this->red_button->pressed_last_frame()) {
        this->input->set_light(ButtonType::Green, true);
        this->input->set_light(ButtonType::Red, false);
        this->input_tick();

        this->set_state(State::Idle);
        this->window_manager->set_countdown_state(CountdownState::Idle);
    }
}

void PhotoBooth::picture_tick() {
    if(chrono::system_clock::now() > this->state_change_time + chrono::seconds(4)) {
        this->set_state(State::Previewing);

        this->window_manager->show_preview("./test.jpeg");
        this->window_manager->hide_flash();
        this->input->set_light(ButtonType::Green, true);
        this->input->set_light(ButtonType::Red, true);
        
        this->window_manager->set_countdown_state(CountdownState::PhotoVerify);
    }
}

void PhotoBooth::preview_tick() {
    if (chrono::system_clock::now() > this->state_change_time + chrono::seconds(60)) {
        this->window_manager->show_blank();
        this->window_manager->hide_preview();
        this->input->set_light(ButtonType::Green, false);
        this->input->set_light(ButtonType::Red, false);
        start_video_capture();
        this->set_state(State::Restarting);
    }

    if (this->green_button->pressed_last_frame()) {
        this->window_manager->show_blank();
        this->window_manager->hide_preview();
        this->input->set_light(ButtonType::Green, false);
        this->input->set_light(ButtonType::Red, false);
        start_video_capture();
        this->set_state(State::Restarting);
    }

    if (this->red_button->pressed_last_frame()) {
        this->window_manager->show_blank();
        this->window_manager->hide_preview();
        this->input->set_light(ButtonType::Green, false);
        this->input->set_light(ButtonType::Red, false);
        start_video_capture();
        this->set_state(State::Restarting);
    }
}

void PhotoBooth::restart_tick() {
    if(chrono::system_clock::now() > this->state_change_time + chrono::seconds(2)) {
        this->set_state(State::Idle);
        this->window_manager->set_countdown_state(CountdownState::Idle);
        this->input->set_light(ButtonType::Green, true);
        this->input->set_light(ButtonType::Red, false);
        this->window_manager->hide_blank();
    }
}

void PhotoBooth::set_state(State state) {
    this->state = state;
    this->state_change_time = chrono::system_clock::now();
}

chrono::milliseconds PhotoBooth::get_milliseconds_since_state_change() {
    return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - this->state_change_time);
}