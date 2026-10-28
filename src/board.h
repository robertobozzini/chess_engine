#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "main.h"

extern Vector2 corner;
extern int squaresize, chessboardsize;

void board_reset(void);
void board_init(void);

void board_draw(void);

#endif // CHESSBOARD_H