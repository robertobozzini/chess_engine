#ifndef EVAL_H
#define EVAL_H
#include "main.h"
#include "board.h"
#include "pieces.h"

extern float eval_val;
extern float target_eval_val;

float evaluate_pos(Position state);
void eval_bar_draw();
#endif