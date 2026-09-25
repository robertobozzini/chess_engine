#ifndef BUTTONS_H
#define BUTTONS_H

#include "main.h"

extern int fontsize;
extern Font uiFont;
extern Color textcolor, hover_textcolor, color, hover_color;
extern float brightness_factor;

struct Button {
    Vector2 position;
    Vector2 size;
    std::string text;
    bool hovered = false;
    bool state = 0;
    void (*on_click)(void);

    void button_event_handler(void);
    void button_drawer(void);
    void init(Vector2 pos, Vector2 sz, std::string txt, void (*click_handler)(void) = nullptr);
};


void buttons_init(void);

#endif