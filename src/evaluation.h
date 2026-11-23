#ifndef EVAL_H
#define EVAL_H
#include "main.h"
#include "board.h"
#include "pieces.h"
#include<thread>
#include<atomic>

enum endgame: int{
    NO_ENDG, TIE, CHECKMATE
};

extern float eval_val;
extern std::atomic<float> target_eval_val;
extern std::atomic<int> search_gen;

extern std::atomic<bool> bot_ready;
extern std::atomic<int> bot_gen;
extern Move bot_move;

void mtx_init(void);
//void sort_moves(std::vector<Move>& moves);
void gen_moves(const Position& state, const Legality& L, std::vector<Move>& moves);
int check_endgame(const Legality& L, const std::vector<Move>& moves, const std::vector<Position>& hist);

float evaluate_pos(Position& state);


void eval_bar_draw();
#endif