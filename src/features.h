#ifndef FEATURES_H

#define FEATURES_H//per evitare recursion
#include "chessboard.h"

using namespace std;

extern vector<pair<Vector2, Vector2>>arrows;
extern bool holdingarrow;

void reset_arrows(Status* status);

void handle_arrows(Status* status, Vector2 mousepos);

void drawArrows(Status* status);

#endif

