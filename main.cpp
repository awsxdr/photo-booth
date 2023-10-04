#include "input.hpp"
#include "mmw.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <signal.h>
#include <string.h>
#include <thread>
#include <X11/Xlib.h>
#include <unistd.h>

using namespace std;
using namespace photo_booth;

Window create_borderless_window(Display* display, int screen, int x, int y, int width, int height, unsigned long background_color);
void set_always_on_top(Display* display, const Window& window);

int main() {

    auto const input = make_unique<Input>();
    input->run_test();

    while(true) {
        auto const red_pressed = input->get_button_pressed(Button::Red);
        auto const green_pressed = input->get_button_pressed(Button::Green);

        cout << (red_pressed ? 1 : 0) << " " << (green_pressed ? 1 : 0) << endl;

        this_thread::sleep_for(chrono::milliseconds(100));
    }

    return 0;

    auto video_process_id = fork();

    if(video_process_id < 0) {
        cout << "Error during fork";
        return -1;
    } else if(video_process_id == 0) {
        execlp("libcamera-vid", "libcamera-vid", "-f", "-t", "0", "--width", "1920", "--height", "1080", "--framerate", "30", "--inline", "--gain", "50", nullptr);
        return 0;
    }

    auto const display = XOpenDisplay(nullptr);
    auto const screen = DefaultScreen(display);

    auto const top_window = create_borderless_window(display, screen, 0, 0, DisplayWidth(display, screen), 100, XBlackPixel(display, screen));    
    auto const bottom_window = create_borderless_window(display, screen, 0, DisplayHeight(display, screen) - 100, DisplayWidth(display, screen), 100, XBlackPixel(display, screen));    
    //auto const flash_window = create_borderless_window(display, screen, 0, 0, DisplayWidth(display, screen), DisplayHeight(display, screen), XWhitePixel(display, screen));

    //XWithdrawWindow(display, flash_window, screen);
    XFlush(display);

    auto const start_time = std::chrono::system_clock::now();

    auto has_taken_picture = false;
    auto is_taking_picture = true;

    XEvent event;
    while(true) {
        if(XPending(display)) {
            XNextEvent(display, &event);
        } else {
            this_thread::sleep_for(chrono::milliseconds(1));
        }

        if(!has_taken_picture && chrono::system_clock::now() > start_time + chrono::seconds(5)) {
            has_taken_picture = true;
            //XMapWindow(display, flash_window);
            //XRaiseWindow(display, flash_window);
            is_taking_picture = true;
            cout << "Taking picture" << endl;

            kill(video_process_id, 1);

            if(fork() == 0) {
                execlp("libcamera-jpeg", "libcamera-jpeg", "-o", "./test.jpeg", "-n", "-t", "1", nullptr);
                return 0;
            }
        }

        if(is_taking_picture && chrono::system_clock::now() > start_time + chrono::seconds(6)) {
            is_taking_picture = false;
            video_process_id = fork();

            if(video_process_id == 0) {
                execlp("libcamera-vid", "libcamera-vid", "-f", "-t", "0", "--width", "1920", "--height", "1080", "--framerate", "30", "--inline", nullptr);
                return 0;
            }

            //XWithdrawWindow(display, flash_window, screen);
            cout << "Picture taken" << endl;
        }
    }

    return 0;
}

Window create_borderless_window(Display* display, int screen, int x, int y, int width, int height, unsigned long background_color) {
    XSetWindowAttributes attributes;
    attributes.background_pixel = background_color;

    auto const window = XCreateWindow(
        display,
        XRootWindow(display, screen),
        x, y, 
        width, height,
        0,
        DefaultDepth(display, screen), 
        InputOutput,
        DefaultVisual(display, screen),
        CWBackPixel,
        &attributes);

    auto const mwm_hints_atom = XInternAtom(display, "_MOTIF_WM_HINTS", 0);
    
    MwmHints hints;
    hints.decorations = 0;
    hints.flags = MWM_HINTS_DECORATIONS;
    
    XChangeProperty(display, window, mwm_hints_atom, mwm_hints_atom, 32, PropModeReplace, reinterpret_cast<unsigned char*>(&hints), 5);

    XFlush(display);    
    XClearWindow(display, window);
    XMapRaised(display, window);
    XFlush(display);    

    set_always_on_top(display, window);

    XMoveWindow(display, window, x, y);

    return window;
}

void set_always_on_top(Display* display, const Window& window) {
    auto const wm_state_above = XInternAtom(display, "_NET_WM_STATE_ABOVE", 1);
    auto const wm_state = XInternAtom(display, "_NET_WM_STATE", 1);

    XClientMessageEvent message_event;
    memset(&message_event, 0, sizeof(message_event));

    message_event.type = ClientMessage;
    message_event.window = window;
    message_event.message_type = wm_state;
    message_event.format = 32;
    message_event.data.l[0] = _NET_WM_STATE_ADD;
    message_event.data.l[1] = wm_state_above;
    message_event.data.l[2] = 0;
    message_event.data.l[3] = 0;
    message_event.data.l[4] = 0;

    XSendEvent(display, DefaultRootWindow(display), False, SubstructureRedirectMask | SubstructureNotifyMask, reinterpret_cast<XEvent*>(&message_event));

    XFlush(display);
}