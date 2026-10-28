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

vector<bool>settings;

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
    //start button -> schermata con impostazioni e altro
    //0 = play, 1 = turn, 2 = bot, 3 = evalbar, 4 = audio
    settings.assign(menu_buttons.size(), false);
    //Settings 0 non serve, 1 = turn, 2 = bot, 3 = eval, 4 = audio
    settings[1] = true; //by default you are playing as white
    settings[2] = false;//by default you are playing alone
    settings[3] = true; //evaluation is on by default
    settings[4] = true; //audio is on by default
    
    

}

bool menu_event_handler(void){
    //gestisce gli eventi della sezione menu
    //ritorna true se si deve cambiare sezione (menu -> game)
    change_section = false;

    buttons_event_handler();
    if(IsKeyPressed(KEY_ESCAPE)){
        if(menu_section == 1)
            menu_section = 0;
        else CloseWindow();
    }
    return change_section;
}


void menu_drawer(void){
    ClearBackground(BLACK);
	
	DrawTextureEx(
		background,
		(Vector2){0, 0},
		0,
		maxscale,
		LIGHTGRAY
	);
	DrawTextureEx(
		logo,
		logo_position,
		0,
		logoscale,
		WHITE
	);

    buttons_draw();

}