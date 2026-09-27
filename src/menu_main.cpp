#include "menu_main.h"

using namespace std;


Texture2D background;
float scaleX, scaleY, maxscale;

Texture2D logo;
float logoscale = 0.5f;
Vector2 logo_position;

//SETTINGS
bool menu_section = 0; //0 = start 1 = settings
bool change_section = false;

vector<Button> menu_buttons;
Button start_button_obj;

vector<bool> settings;

void menu_init(void){
    //carica le texture, font, ecc della sezione menu
    //inizializza la sezione menu
    
	background = LoadTexture("../images/background.png");
	logo = LoadTexture("../images/LOGO.png");
	
	scaleX = (float)GetScreenWidth() / background.width;
	scaleY = (float)GetScreenHeight() / background.height;
	maxscale = fmaxf(scaleX,scaleY);
	
	
	logo_position.x=GetScreenWidth()/2 - logo.width*logoscale/2;
	logo_position.y=0;
	
	menu_section = 0;
	//uiFont = LoadFontEx("../Fonts/Inter/static/Inter_28pt-SemiBold.ttf", fontsize, NULL, 0);

    buttons_init();
    settings.assign(menu_buttons.size(), false);
    //Settings 0 non serve, 1 = turn, 2 = bot, 3 = eval, 4 = audio
    settings[1] = true; //by default you are playing as white
    settings[3] = true; //eval button is on by default
    settings[4] = true; //audio button is on by default
    
    

}

bool menu_event_handler(void){
    //gestisce gli eventi della sezione menu
    //ritorna true se si deve cambiare sezione (menu -> game)
    change_section = false;

    buttons_event_handler();


    return change_section;
}


void menu_drawer(void){
    //disegna la sezione menu
    if(menu_section == 0){

    }
    else{
        for (auto &button : menu_buttons){
            button.button_drawer();
        }
    }
}