


#ifndef CHESSP_H

#define CHESSP_H//per evitare recursion
#include "main.h"
using namespace std;

extern Texture2D bp;
extern Texture2D bn;
extern Texture2D bb;
extern Texture2D br;
extern Texture2D bq;
extern Texture2D bk;

extern Texture2D wp;
extern Texture2D wn;
extern Texture2D wb;
extern Texture2D wr;
extern Texture2D wq;
extern Texture2D wk;

extern float scalepiece;
struct piece{
	int type=0; 
//in modo da poter fare la scacchiera come una matrice di pezzi
	/*
	0 = vuoto
	1 = pawn
	2 = night
	3 = bishop
	4 = rook 
	5 = queen
	6 = king
	*/
	bool col=2;//0=nero 1=bianco
	
	bool onboard=1;//è fermo sulla scacchiera
	
	Vector2 pos = (Vector2){0,0};//riga e colonna nella scacchiera 
	//(in modo che non è bindato alla scacchiera)
	bool moved = 0;
	
	vector<Vector2>possible_moves;
	
	Texture2D img;
	
	int enpaspos = -1;//puoi avere solo un en passant per turno per pezzo (max 2 per turno totali)
	void draw(Status* status);
	void calculate_moves(void);
};

bool would_be_check(bool col, Vector2 pos = Vector2{-1,-1}, Vector2 newpos = Vector2{-1,-1});
bool check_king_checks(bool col);
#endif

