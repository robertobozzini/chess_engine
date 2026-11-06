#ifndef BOARD_FEATURES_H
#define BOARD_FEATURES_H

#include "main.h"
#include <cstdint>

extern u_int8_t held_piece;
extern int held_piece_sq;
extern std::vector<std::pair<Vector2,Vector2>>arrows;
void handle_mouse(void);

void draw_features(void);
#endif // BOARD_FEATURES_H