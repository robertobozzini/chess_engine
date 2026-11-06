#ifndef PIECES_H
#define PIECES_H
#include "main.h"

enum Piece : u_int8_t { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, EMPTY};
extern Texture2D texall[2][6];
extern int piece_value[7];
extern float scalepiece;

void load_pieces_textures(void);
void draw_piece(bool turn, u_int8_t piece, Vector2 pos);
void pieces_draw(bool turn);
#endif