#include "countdown_window.hpp"
#include "texture_loader.hpp"

#include <exception>
#include <fstream>
#include <GL/gl.h>
#include <GL/glx.h>
#include <iostream>
#include <sstream>
#include <string>

using namespace photo_booth;
using namespace std;

CountdownWindow::CountdownWindow(std::shared_ptr<Display> display, Window window, XVisualInfo* visual_info) :
    texture_loader(make_unique<TextureLoader>()),
    display(display),
    window(window),
    visual_info(visual_info),
    state_update_time(chrono::system_clock::now()),
    state(CountdownState::Starting)
{
    this->context = glXCreateContext(this->display.get(), this->visual_info, nullptr, GL_TRUE);
    this->load_textures();
}

CountdownWindow::~CountdownWindow() {
    glXMakeCurrent(this->display.get(), this->window, this->context);

    glDeleteTextures(1, &this->textures[TextureType::Number1]);
    glDeleteTextures(1, &this->textures[TextureType::Number2]);
    glDeleteTextures(1, &this->textures[TextureType::Number3]);
    glDeleteTextures(1, &this->textures[TextureType::Number4]);
    glDeleteTextures(1, &this->textures[TextureType::Number5]);
    glDeleteTextures(1, &this->textures[TextureType::GetReady]);
    glDeleteTextures(1, &this->textures[TextureType::Review]);
    glDeleteTextures(1, &this->textures[TextureType::Start]);

    glXDestroyContext(this->display.get(), this->context);
}

void CountdownWindow::render() {
    glXMakeCurrent(this->display.get(), this->window, this->context);

    XWindowAttributes window_attributes;
    XGetWindowAttributes(this->display.get(), this->window, &window_attributes);

    glViewport(0, 0, window_attributes.width, window_attributes.height);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, window_attributes.width, window_attributes.height, 0.0, 0.0, 20.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -10.0f);

    switch(this->state) {
    case CountdownState::Starting:
        render_starting();
        break;

    case CountdownState::Idle:
        render_idle();
        break;

    case CountdownState::CountingDown:
        render_countdown();
        break;

    case CountdownState::PhotoVerify:
        render_verify();
        break;
    }

    glXSwapBuffers(this->display.get(), this->window);
}

void CountdownWindow::set_state(CountdownState state) {
    this->state = state;
    this->state_update_time = chrono::system_clock::now();
}

void CountdownWindow::render_starting() {
}

void CountdownWindow::render_idle() {
    glBindTexture(GL_TEXTURE_2D, this->textures[TextureType::Start]);
    glBegin(GL_QUADS);
    {
        glVertex3f(0.0,     0.0,    0.0);   glTexCoord2f(1.0, 0.0);
        glVertex3f(1920.0,  0.0,    0.0);   glTexCoord2f(1.0, 1.0);
        glVertex3f(1920.0,  100.0,  0.0);   glTexCoord2f(0.0, 1.0);
        glVertex3f(0.0,     100.0,  0.0);   glTexCoord2f(0.0, 0.0);
    }
    glEnd();
}

void CountdownWindow::render_countdown() {
    glBindTexture(GL_TEXTURE_2D, this->textures[TextureType::GetReady]);
    glBegin(GL_QUADS);
    {
        glVertex3f(0.0,     0.0,    0.0);   glTexCoord2f(1.0, 0.0);
        glVertex3f(1920.0,  0.0,    0.0);   glTexCoord2f(1.0, 1.0);
        glVertex3f(1920.0,  100.0,  0.0);   glTexCoord2f(0.0, 1.0);
        glVertex3f(0.0,     100.0,  0.0);   glTexCoord2f(0.0, 0.0);
    }
    glEnd();

    auto const time_since_state_update = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - this->state_update_time).count();

    auto const number_texture = 
        time_since_state_update == 4 ? TextureType::Number1
        : time_since_state_update == 3 ? TextureType::Number2
        : time_since_state_update == 2 ? TextureType::Number3
        : time_since_state_update == 1 ? TextureType::Number4
        : TextureType::Number5;
    glBindTexture(GL_TEXTURE_2D, this->textures[number_texture]);
    glBegin(GL_QUADS);
    {
        glVertex3f(0.0,     0.0,    0.0);   glTexCoord2f(1.0, 0.0);
        glVertex3f(100.0,   0.0,    0.0);   glTexCoord2f(1.0, 1.0);
        glVertex3f(100.0,   100.0,  0.0);   glTexCoord2f(0.0, 1.0);
        glVertex3f(0.0,     100.0,  0.0);   glTexCoord2f(0.0, 0.0);
    }
    glEnd();

    glBegin(GL_QUADS);
    {
        glVertex3f(1820.0,  0.0,    0.0);   glTexCoord2f(1.0, 0.0);
        glVertex3f(1920.0,  0.0,    0.0);   glTexCoord2f(1.0, 1.0);
        glVertex3f(1920.0,  100.0,  0.0);   glTexCoord2f(0.0, 1.0);
        glVertex3f(1820.0,  100.0,  0.0);   glTexCoord2f(0.0, 0.0);
    }
    glEnd();
}

void CountdownWindow::render_verify() {
    glBindTexture(GL_TEXTURE_2D, this->textures[TextureType::Review]);
    glBegin(GL_QUADS);
    {
        glVertex3f(0.0,     0.0,    0.0);   glTexCoord2f(1.0, 0.0);
        glVertex3f(1920.0,  0.0,    0.0);   glTexCoord2f(1.0, 1.0);
        glVertex3f(1920.0,  100.0,  0.0);   glTexCoord2f(0.0, 1.0);
        glVertex3f(0.0,     100.0,  0.0);   glTexCoord2f(0.0, 0.0);
    }
    glEnd();
}

void CountdownWindow::load_textures() {
    glXMakeCurrent(this->display.get(), this->window, this->context);

    this->textures = {
        { TextureType::Number1, this->texture_loader->load_png("./textures/1.png") },
        { TextureType::Number2, this->texture_loader->load_png("./textures/2.png") },
        { TextureType::Number3, this->texture_loader->load_png("./textures/3.png") },
        { TextureType::Number4, this->texture_loader->load_png("./textures/4.png") },
        { TextureType::Number5, this->texture_loader->load_png("./textures/5.png") },
        { TextureType::Start, this->texture_loader->load_png("./textures/start.png") },
        { TextureType::GetReady, this->texture_loader->load_png("./textures/get_ready.png") },
        { TextureType::Review, this->texture_loader->load_png("./textures/review.png") }
    };
}
