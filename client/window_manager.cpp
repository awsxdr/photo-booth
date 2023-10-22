#include "window_manager.hpp"
#include "countdown_window.hpp"
#include "preview_window.hpp"
#include "mmw.h"

#include <iostream>
#include <string.h>
#include <X11/Xlib.h>

using namespace photo_booth;
using namespace std;

WindowManager::WindowManager() :
    display(XOpenDisplay(nullptr), XCloseDisplay),
    screen(DefaultScreen(display.get())),
    display_size({ DisplayWidth(this->display.get(), this->screen), DisplayHeight(this->display.get(), this->screen) }),
    is_preview_visible(false)
{
    GLint gl_attributes[] = {
        GLX_RGBA,
        GLX_DOUBLEBUFFER, 
        None
    };

    auto const visual_info = glXChooseVisual(display.get(), DefaultScreen(display.get()), gl_attributes);
    auto const white_pixel = XWhitePixel(this->display.get(), this->screen);
    auto const black_pixel = XBlackPixel(this->display.get(), this->screen);
    
    this->top_bar = this->create_borderless_window(
        0, 0, 
        this->display_size.width, 100,
        visual_info->depth,
        visual_info->visual,
        black_pixel);

    this->bottom_bar = this->create_borderless_window(
        0, this->display_size.height - 100,
        display_size.width, 100,
        visual_info->depth,
        visual_info->visual,
        black_pixel);

    this->flash_window = this->create_borderless_window(
        0, 0,
        this->display_size.width, 1,
        DefaultDepth(this->display.get(), screen),
        DefaultVisual(this->display.get(), screen),
        white_pixel);
    
    this->blank_window = this->create_borderless_window(
        0, 0,
        this->display_size.width, 1,
        DefaultDepth(this->display.get(), screen),
        DefaultVisual(this->display.get(), screen),
        black_pixel);
    
    this->preview_window = this->create_borderless_window(
        0, 0,
        this->display_size.width, 1,
        DefaultDepth(this->display.get(), screen),
        DefaultVisual(this->display.get(), screen),
        black_pixel);

    XFlush(this->display.get());

    this->top_countdown = make_unique<CountdownWindow>(this->display, this->top_bar, visual_info);
    this->bottom_countdown = make_unique<CountdownWindow>(this->display, this->bottom_bar, visual_info);
    this->preview = make_unique<PreviewWindow>(this->display, this->preview_window, visual_info);
}

WindowManager::~WindowManager() {
    XDestroyWindow(this->display.get(), this->top_bar);
    XDestroyWindow(this->display.get(), this->bottom_bar);
    XDestroyWindow(this->display.get(), this->flash_window);
    XDestroyWindow(this->display.get(), this->blank_window);
    XDestroyWindow(this->display.get(), this->preview_window);
}

void WindowManager::show_flash() const {
    XResizeWindow(this->display.get(), this->flash_window, this->display_size.width, this->display_size.height);
    XRaiseWindow(this->display.get(), this->flash_window);
    XFlush(this->display.get());
}

void WindowManager::hide_flash() const {
    XResizeWindow(this->display.get(), this->flash_window, this->display_size.width, 1);
    XFlush(this->display.get());
}

void WindowManager::show_blank() const {
    XResizeWindow(this->display.get(), this->blank_window, this->display_size.width, this->display_size.height);
    XRaiseWindow(this->display.get(), this->blank_window);
    XFlush(this->display.get());
}

void WindowManager::hide_blank() const {
    XResizeWindow(this->display.get(), this->blank_window, this->display_size.width, 1);
    XFlush(this->display.get());
}

void WindowManager::show_preview(string path) {
    this->preview->load_preview(path);
    is_preview_visible = true;

    XResizeWindow(this->display.get(), this->preview_window, this->display_size.width, this->display_size.height);
    XRaiseWindow(this->display.get(), this->preview_window);
    XRaiseWindow(this->display.get(), this->top_bar);
    XRaiseWindow(this->display.get(), this->bottom_bar);
    XFlush(this->display.get());
}

void WindowManager::hide_preview() {
    XResizeWindow(this->display.get(), this->preview_window, this->display_size.width, 1);
    XFlush(this->display.get());
    is_preview_visible = false;
}

bool WindowManager::events_tick() const {
    if(XPending(display.get())) {
        XEvent event;
        XNextEvent(display.get(), &event);

        return true;
    }

    return false;
}

void WindowManager::render() const {
    this->top_countdown->render();
    this->bottom_countdown->render();

    if(this->is_preview_visible) {
        this->preview->render();
    }
}

void WindowManager::set_countdown_state(CountdownState state) {
    this->top_countdown->set_state(state);
    this->bottom_countdown->set_state(state);
}

Window WindowManager::create_borderless_window(int x, int y, int width, int height, int depth, Visual* visual, unsigned long background_color) const {
    XSetWindowAttributes attributes;
    attributes.background_pixel = background_color;
    attributes.event_mask = ExposureMask;
    attributes.colormap = XCreateColormap(this->display.get(), DefaultRootWindow(this->display.get()), visual, AllocNone);

    auto const window = XCreateWindow(
        this->display.get(),
        XRootWindow(this->display.get(), screen),
        x, y, 
        width, height,
        0,
        depth, 
        InputOutput,
        visual,
        CWBackPixel | CWEventMask | CWColormap,
        &attributes);

    auto const mwm_hints_atom = XInternAtom(this->display.get(), "_MOTIF_WM_HINTS", 0);
    
    MwmHints hints;
    hints.decorations = 0;
    hints.flags = MWM_HINTS_DECORATIONS;
    
    XChangeProperty(this->display.get(), window, mwm_hints_atom, mwm_hints_atom, 32, PropModeReplace, reinterpret_cast<unsigned char*>(&hints), 5);
    XFlush(this->display.get());    

    XClearWindow(this->display.get(), window);
    XMapRaised(this->display.get(), window);
    XFlush(this->display.get());    

    set_always_on_top(window);

    XMoveWindow(this->display.get(), window, x, y);

    return window;
}

void WindowManager::set_always_on_top(const Window& window) const {
    auto const wm_state_above = XInternAtom(this->display.get(), "_NET_WM_STATE_ABOVE", 1);
    auto const wm_state = XInternAtom(this->display.get(), "_NET_WM_STATE", 1);

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

    XSendEvent(this->display.get(), DefaultRootWindow(this->display.get()), False, SubstructureRedirectMask | SubstructureNotifyMask, reinterpret_cast<XEvent*>(&message_event));

    XFlush(this->display.get());
}

