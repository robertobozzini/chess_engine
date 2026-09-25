#include "menu_main.h"

using namespace std;

Texture2D background;
float scaleX, scaleY, maxscale;

Texture2D logo;
float logoscale = 0.5f;
Vector2 logo_position;

int fontsize = 32;
void menu_init(void){
    //carica le texture, font, ecc della sezione menu
    //inizializza la sezione menu
    
	background = LoadTexture("../images/background.png");
	logo = LoadTexture("../images/LOGO.png");
	
	scaleX = (float)GetScreenWidth() / background.width;
	scaleY = (float)GetScreenHeight() / background.height;
	maxscale=fmaxf(scaleX,scaleY);
	
	
	logo_position.x=GetScreenWidth()/2 - logo.width*logoscale/2;
	logo_position.y=0;
	
	
	uiFont = LoadFontEx("../Fonts/Inter/static/Inter_28pt-SemiBold.ttf", fontsize, NULL, 0);
}

bool menu_event_handler(void){
    //gestisce gli eventi della sezione menu
    //ritorna true se si deve cambiare sezione (menu -> game)
    bool change_section = false;



    return change_section;
}


void menu_drawer(void){
    //disegna la sezione menu
}