#include "preview_window.hpp"

#include <memory>
#include <string>

using namespace photo_booth;
using namespace std;

PreviewWindow::PreviewWindow(std::shared_ptr<Display> display, Window window, XVisualInfo* visual_info) :
    texture_loader(make_unique<TextureLoader>()),
    display(display),
    window(window),
    visual_info(visual_info)
{
    this->context = glXCreateContext(this->display.get(), this->visual_info, nullptr, GL_TRUE);
}

PreviewWindow::~PreviewWindow() {
    glXMakeCurrent(this->display.get(), this->window, this->context);
    if(this->preview_texture) {
        glDeleteTextures(1, &this->preview_texture);
    }
    glXDestroyContext(this->display.get(), this->context);
}

void PreviewWindow::render() {
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

    glBindTexture(GL_TEXTURE_2D, this->preview_texture);
    glBegin(GL_QUADS);
    {
        glVertex3f(0.0,     0.0,    0.0);   glTexCoord2f(1.0, 0.0);
        glVertex3f(1920.0,  0.0,    0.0);   glTexCoord2f(1.0, 1.0);
        glVertex3f(1920.0,  1080.0, 0.0);   glTexCoord2f(0.0, 1.0);
        glVertex3f(0.0,     1080.0, 0.0);   glTexCoord2f(0.0, 0.0);
    }
    glEnd();

    glXSwapBuffers(this->display.get(), this->window);
}

void PreviewWindow::load_preview(string path) {
    glXMakeCurrent(this->display.get(), this->window, this->context);
    if(this->preview_texture) {
        glDeleteTextures(1, &this->preview_texture);
    }
    this->preview_texture = this->texture_loader->load_jpeg(path);
}