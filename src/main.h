#ifndef MAIN_H

#define MAIN_H//per evitare recursion

#include<raylib.h>
#include<vector>
#include<raymath.h>
#include<iostream>
#include<algorithm>
#include<cstdint>

//for the arrow set
inline bool operator<(const Vector2 &a, const Vector2 &b){ return a.x < b.x || (a.x == b.x && a.y < b.y); }

extern int ScreenWidth;
extern int ScreenHeight;

extern bool (*event_handler)(void);//coso figo
//bool in modo che mi restituisca true se devo cambiare sezione, false altrimenti
extern void (*screen_drawer)(void);

extern Vector2 mouse_position;

#endif