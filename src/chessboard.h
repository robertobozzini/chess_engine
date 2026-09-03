#ifndef CHESS_H
#define CHESS_H//per evitare recursion
#include "main.h"
#include "chesspieces.h"
#include "features.h"

using namespace std;
//il problema è sto file
extern vector<vector<piece>>pieces;

extern int squaresize, chessboardsize;
extern Vector2 corner;

extern Vector2 kingposw;
extern Vector2 kingposb;

extern bool choosing;
void chessboard_init(Status *status);

void chessboard_reset(Status *status);

void chessboard_handler(Status *status);

void chessboard_drawer(Status *status);

bool checklegalmove(Status *status, Vector2 from, Vector2 to);

bool isinboard(Vector2 pos);

bool check_endgame(bool color);

Vector2 chessboardpos(Vector2 mousepos);

void makemove(bool turn,Vector2 oldpos, Vector2 newpos);

#endif

