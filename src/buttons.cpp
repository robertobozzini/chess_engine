#include "buttons.h"

using namespace std;

int fontsize = 32;
Font uiFont;

Color textcolor;
Color hover_textcolor;

Color color;
Color hover_color;

float brightness_factor = -0.15f;

struct Button {

    Vector2 position;
    Vector2 size;

    string text;

    bool hovered = false;

    bool state = 0; //spento

    void (*on_click)(void);

    void button_event_handler(void){

        Vector2 mouse_pos = GetMousePosition();

        hovered = CheckCollisionPointRec(mouse_pos, {position.x, position.y, size.x, size.y});

        if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            state = !state;
            if(on_click != nullptr){
                on_click();
            }
        }
    }

    void button_drawer(void){

        int mulfactor = state ? -2 : 1;

        Color draw_color = hovered ? hover_color : color;
        draw_color = ColorBrightness(draw_color, brightness_factor * mulfactor);
        DrawRectangleV(position, size, draw_color);

        Vector2 text_size = MeasureTextEx(uiFont, text.c_str(), fontsize, 0);
        Vector2 text_position = {position.x + size.x/2 - text_size.x/2, position.y + size.y/2 - text_size.y/2};

        Color text_draw_color = hovered ? hover_textcolor : textcolor;
        text_draw_color = ColorBrightness(text_draw_color, brightness_factor * mulfactor);
        DrawTextEx(uiFont, text.c_str(), text_position, fontsize, 0, text_draw_color);
    }

    void init(Vector2 pos, Vector2 sz, string txt, void (*click_handler)(void) = nullptr){
        position = pos;
        size = sz;
        text = txt;
        on_click = click_handler;
    }


};

void buttons_init(void){
    //inizializza i bottoni
    uiFont = LoadFontEx("../Fonts/Inter/static/Inter_28pt-SemiBold.ttf", fontsize, NULL, 0);

    //buttons.clear();

    color = BEIGE;
    hover_color = ColorBrightness(color, brightness_factor);
    textcolor = DARKBROWN;
    hover_textcolor = ColorBrightness(textcolor, brightness_factor);
}