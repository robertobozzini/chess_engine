#ifndef PIECES_H
#define PIECES_H
#include "main.h"
#include "board.h"

enum Piece : u_int8_t { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, EMPTY};
extern Texture2D texall[2][6];
extern int piece_value[7];
extern float scalepiece;

struct Move { uint8_t from, to, piece, captured, promo, flag; }; // captured/promo = EMPTY se assenti
enum MoveFlag : uint8_t { NORMAL, DOUBLE_PUSH, EN_PASSANT, CASTLE };

enum Dir: u_int8_t {E, W, N, S, NE, NW, SE, SW}; //e = east, w = west, n = north, s = south, ne = north-east, ...
extern int direction[8];//quanto devi aggiungere ad un quadrato per spostarlo in quella direzione

extern uint64_t pawn_att[2][64], knight_att[64], king_att[64], ray[8][64]; //attack tables

void att_tables_init(void);

uint64_t find_illegal(Position state);
uint64_t find_moves(Position state, u_int8_t piece, int sq);

void load_pieces_textures(void);
void draw_piece(bool turn, u_int8_t piece, Vector2 pos);
void pieces_draw(bool turn);
#endif