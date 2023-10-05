#include "input.hpp"
#include "mmw.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <X11/Xlib.h>
#include <unistd.h>

using namespace std;
using namespace photo_booth;

Window create_borderless_window(Display* display, int screen, int x, int y, int width, int height, unsigned long background_color);
void set_always_on_top(Display* display, const Window& window);
int start_video_capture();

int main() {
    auto const input = make_unique<Input>();
    input->run_test();
    auto video_process_id = start_video_capture();

    auto const display = XOpenDisplay(nullptr);
    auto const screen = DefaultScreen(display);

    auto const top_window = create_borderless_window(display, screen, 0, 0, DisplayWidth(display, screen), 100, XBlackPixel(display, screen));    
    auto const bottom_window = create_borderless_window(display, screen, 0, DisplayHeight(display, screen) - 100, DisplayWidth(display, screen), 100, XBlackPixel(display, screen));    

    XFlush(display);

    auto picture_start_time = chrono::system_clock::now();
    auto is_taking_picture = false;
    auto green_down = false;

    XEvent event;
    while(true) {
        if(XPending(display)) {
            XNextEvent(display, &event);
        } else {
            this_thread::sleep_for(chrono::milliseconds(1));
        }

        auto const new_green_down = input->get_button_pressed(Button::Green);

        if(!is_taking_picture && green_down && !new_green_down) {
            is_taking_picture = true;
            picture_start_time = chrono::system_clock::now();
            cout << "Taking picture" << endl;

            kill(video_process_id, 1);

            if(fork() == 0) {
                execlp("libcamera-jpeg", "libcamera-jpeg", "-o", "./test.jpeg", "-n", "-t", "1", nullptr);
                return 0;
            }
        }

        green_down = new_green_down;

        if(is_taking_picture && chrono::system_clock::now() > picture_start_time + chrono::seconds(1)) {
            is_taking_picture = false;
            video_process_id = start_video_capture();

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

int start_video_capture() {
    auto const process_id = fork();

    if(process_id == 0) {
        execlp("libcamera-vid", "libcamera-vid", "-f", "-t", "0", "--width", "1920", "--height", "1080", "--framerate", "30", "--inline", "--gain", "50", nullptr);
        exit(0);
    }

    return process_id;
}