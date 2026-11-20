#ifndef EVAL_H
#define EVAL_H
#include "main.h"
#include "board.h"
#include "pieces.h"

enum endgame: int{
    NO_ENDG, TIE, CHECKMATE
};

extern float eval_val;
extern float target_eval_val;

void sort_moves(std::vector<Move>& moves);
void gen_moves(const Position& state, const Legality& L, std::vector<Move>& moves);
int check_endgame(const Legality& L, const std::vector<Move>& moves);

float try_moves(int depth, Position state, const Move &mv);
float evaluate_pos(Position& state);
void eval_bar_draw();
#endif