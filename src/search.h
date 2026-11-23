#ifndef SEARCH_H
#define SEARCH_H
#include "main.h"
#include "board.h"
#include "pieces.h"
#include<thread>
#include<atomic>


extern std::atomic<int> search_gen;

extern std::atomic<bool> bot_ready;
extern std::atomic<int> bot_gen;
extern Move bot_move;

//void sort_moves(std::vector<Move>& moves);

float try_moves(int depth, Position state, const Move &mv, std::vector<Position>& hist, float alpha, float beta, int id);

void eval_asyinc(Position& state);

void do_best_move(const Position& state,const Legality& leg, std::vector<Position>&hist, int id);
void bot_asyinc(const Position& state, const Legality &leg);

#endif