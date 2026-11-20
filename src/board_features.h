#ifndef BOARD_FEATURES_H
#define BOARD_FEATURES_H

#include "main.h"
#include <cstdint>
#include "buttons.h"

extern uint8_t held_piece;
extern int held_piece_sq;
extern std::vector<std::pair<Vector2,Vector2>>arrows;

extern Font board_font;
extern Font text_font;

extern Button take_back_button;
extern Button restart_button;
extern Button lobby_button;
void game_buttons_init(void);

void handle_mouse(void);
bool handle_features(void);
void draw_features(void);
#endif // BOARD_FEATURES_H