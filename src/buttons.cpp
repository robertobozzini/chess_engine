#include "buttons.h"
#include "menu_main.h" 
#include "buttons_specifics.h"
using namespace std;



Color textcolor;
Color hover_textcolor;

Color color;
Color hover_color;

float brightness_factor = -0.15f;



void Button::button_event_handler(void){

    Vector2 mouse_pos = GetMousePosition();

    hovered = CheckCollisionPointRec(mouse_pos, {position.x, position.y, size.x, size.y});

    if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        state = !state;
        if(on_click != nullptr){
            on_click();
        }
    }
}

void Button::button_drawer(void){

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

void Button::init(Vector2 pos, Vector2 sz, std::string txt, int _fontsize, Font _uiFont, void (*click_handler)(void)){
    position = pos;
    size = sz;
    text = txt;
    fontsize = _fontsize;
    uiFont = _uiFont;
    on_click = click_handler;
}




void buttons_init(void){
    //inizializza i bottoni

    color = BEIGE;
    hover_color = ColorBrightness(color, brightness_factor);
    textcolor = DARKBROWN;
    hover_textcolor = ColorBrightness(textcolor, brightness_factor);

    int fontsize;
    Font uiFont;
    Vector2 button_size;

    fontsize = 50;
    uiFont = LoadFontEx("../Fonts/Inter/static/Inter_28pt-ExtraBold.ttf", fontsize, NULL, 0);

    button_size.x = ScreenWidth/3.0f;
    button_size.y = ScreenHeight/8.0f;

    start_button_obj.init(Vector2{ScreenWidth/2.0f - button_size.x/2.0f, ScreenHeight/2.0f}, button_size, "START", fontsize, uiFont, start_button);

    //normal buttons

    fontsize = 32;
    

    button_size.x = ScreenWidth/4.0f;
    button_size.y = ScreenHeight/12.0f;
    //menu butttons: //0 = play, 1 = turn, 2 = bot, 3 = evalbar, 4 = audio
    menu_buttons.clear();
    Button tmp;
    Vector2 button_pos;
    button_pos.x = ScreenWidth*1.0f/4.0f - button_size.x/2.0f;
    button_pos.y = ScreenHeight/5.0f + 2.0f*button_size.y;

    tmp.init(button_pos, button_size,"Play", fontsize, uiFont, play_button);
    menu_buttons.push_back(tmp);

    //settings
    button_pos.x = ScreenWidth*3.0f/4.0f - button_size.x/2.0f;
    uiFont = LoadFontEx("../Fonts/Inter/static/Inter_28pt-Bold.ttf", fontsize, NULL, 0);


    //button_pos.y += 2.0f*button_size.y;
    tmp.init(button_pos, button_size,"Playing as White", fontsize, uiFont, turn_button);
    menu_buttons.push_back(tmp);

    button_pos.y += 2.0f*button_size.y;
    tmp.init(button_pos, button_size,"Playing with a friend", fontsize, uiFont, bot_button);
    menu_buttons.push_back(tmp);

    button_pos.y += 2.0f*button_size.y;
    tmp.init(button_pos, button_size,"Evaluation bar active", fontsize, uiFont, eval_button);
    menu_buttons.push_back(tmp);

    button_pos.y += 2.0f*button_size.y;
    tmp.init(button_pos, button_size,"Audio active", fontsize, uiFont, audio_button);
    menu_buttons.push_back(tmp);

}

void buttons_event_handler(void){
    //gestisce gli eventi dei bottoni
    if(menu_section == 0){
        start_button_obj.button_event_handler();
    }
    else{//menu_section == 1
        for (Button& button : menu_buttons){
            button.button_event_handler();
        }
    }

}

void buttons_draw(void){
    if(menu_section == 0){
        start_button_obj.button_drawer();
    }
    else{
        for (auto &button : menu_buttons){
            button.button_drawer();
        }
    }
}