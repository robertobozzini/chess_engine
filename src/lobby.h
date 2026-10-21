
#ifndef LOBBY_H
#define LOBBY_H//per evitare recursion
#include "main.h"
extern Texture2D background;
extern int stage;

void lobby_init(Status *status);

void lobby_handler(Status *status);

void lobby_drawer(Status *status);

#endif

