#ifndef MAIN_H

#define MAIN_H//per evitare recursion

#include<raylib.h>
#include<vector>
#include<raymath.h>
#include<iostream>
#include <algorithm>


extern int ScreenWidth;
extern int ScreenHeight;

extern bool (*event_handler)(void);//coso figo
extern void (*screen_drawer)(void);

extern Vector2 mouse_position;
#endif