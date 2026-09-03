#ifndef EVAL_H
#define EVAL_H//per evitare recursion

#include "chessboard.h"
#include "lobby.h"

struct record{
	
	piece taking,taken;
	
	bool moved_set = 0;
	
	bool wascastle = 0;
	bool wasenpas = 0;
	
	Vector2 rookfrom, rookto;
	
};

extern float target_eval_val;
extern int max_depth;

extern vector<record>moves;

void undo_move(void);

float evaluate_pieces(bool turn);

float try_move(bool turn, int depth = 0, float alphap = -106.0f, float betap = 106.0f);

void eval_bar_draw(Status *status);
#endif
