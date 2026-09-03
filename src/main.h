
#ifndef MAIN_H

#define MAIN_H//per evitare recursion


#include<raylib.h>
#include<vector>
#include<raymath.h>
#include<iostream>

extern Texture2D cursorb;
extern Texture2D cursorh;

extern int ScreenWidth;
extern int ScreenHeight;
struct Status{
	bool turn = 1;//1=bianco
	bool ingame = 0;
	bool isholding = 0;
	bool inverted = 0;
	
	bool bot = 1;
	
	bool audio = 1;
	
};

extern void (*event_handler)(Status *status);//coso figo
extern void (*screen_drawer)(Status *status);



#endif

