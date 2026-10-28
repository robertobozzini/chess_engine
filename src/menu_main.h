#ifndef MENU_MAIN_H
#define MENU_MAIN_H

#include "main.h"
#include "buttons.h"

using namespace std;

extern Texture2D logo;

extern vector<Button>menu_buttons;
extern Button start_button_obj;

extern bool menu_section; //0 = start 1 = settings
extern bool change_section;

extern vector<bool> settings;

void menu_init(void);

bool menu_event_handler(void);

void menu_drawer(void);
#endif