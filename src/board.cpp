#include "board.h"
#include "game_main.h"
#include "menu_main.h"
#include <bit>
#include <vector>

using namespace std;



Vector2 corner;
int squaresize, chessboardsize;

Texture2D texall[2][6];

vector<Position>record;

Position startingp;
Position currentp;
Position emptyp;

inline int lsb(U64 bb) { 
	if(bb == 0)
		return 64;//thecnically its -1 but i cant
	return __builtin_ctzll(bb); 
}//HAVE TO CHECK ITS NOT 64 when i use it: almost impossible but who knows
inline int msb(U64 bb) { 
	if(bb == 0)
		return 64;//correct
	return 63 - __builtin_clzll(bb); //63 - leading 0s
}

inline int pop_lsb(U64& bb) { 
	int s = lsb(bb); 
	bb &= bb - 1; 
	return s; 
} 

inline bool is_occ(U64 bb, int sq) { return bb >> sq & 1; }//checks if square number sq is occupied in board bb
inline void set_bit(U64& bb, int sq)   { bb |= 1ULL << sq; }
inline void clear_bit(U64& bb, int sq) { bb &= ~(1ULL << sq); }

/*reminder: bitwise operations
& = and
| = or
~ = not
^ = xor
*/

void board_reset(void){
    record.clear();
	//record.push_back(startingp);
	currentp = startingp;
}

void set_board_piece(Position &board, bool color, int sq, int piece){
	set_bit(board.pieces[color][piece],sq);
	set_bit(board.occ[color],sq);
	set_bit(board.all,sq);
}
int invert_onb(int sq){
	//return 7*8-v;
	//devo invertire da un lato all'altro: A1 diventa H1 e viceversa
	return 0;
}
void set_starting_pos(bool color){
	//pezzi singoli -> cambi anche typeon
	//devo fare cosa simmetrica rispetto alla riga in messo: row = 0 = 7-> media e 3.5
	//0 -> 7
	//1 -> 6
	//a -> (7-a)
	//ovviamente io vado su tutto non solo questo
	//quindi 7-> 8*7 ? peso

	int sq, piece;
	piece = PAWN;
	for(int i=B1;i<B8;i++){//pawns
		sq = (color)?i:invert_onb(i);
		set_board_piece(startingp, color, sq, piece);
	}
	//knights
	//pos 2 e 7
	piece = KNIGHT;
	sq = A2;
	sq = (color)?sq:invert_onb(sq);
	set_board_piece(startingp, color, sq, piece);

	sq = A7;
	sq = (color)?sq:invert_onb(sq);
	set_board_piece(startingp, color, sq, piece);

	//bishop
	//pos 3 e 6
	piece = BISHOP;
	sq = A3;
	sq = (color)?sq:invert_onb(sq);
	set_board_piece(startingp, color, sq, piece);

	sq = A6;
	sq = (color)?sq:invert_onb(sq);
	set_board_piece(startingp, color, sq, piece);

	//rook
	//pos 1 e 8
	piece = ROOK;
	sq = A1;
	sq = (color)?sq:invert_onb(sq);
	set_board_piece(startingp, color, sq, piece);

	sq = A8;
	sq = (color)?sq:invert_onb(sq);
	set_board_piece(startingp, color, sq, piece);

}

void board_init(void){
    corner.y=(ScreenHeight)/9;
	squaresize=corner.y;
	corner.y/=2;
	chessboardsize=squaresize*8;
	corner.x = (ScreenWidth - chessboardsize ) - corner.y;

    texall[0][0] = LoadTexture("../images/bp.png");
	texall[0][1] = LoadTexture("../images/bn.png");
	texall[0][2] = LoadTexture("../images/bb.png");
	texall[0][3] = LoadTexture("../images/br.png");
	texall[0][4] = LoadTexture("../images/bq.png");
	texall[0][5] = LoadTexture("../images/bk.png");
	
	texall[1][0] = LoadTexture("../images/wp.png");
	texall[1][1] = LoadTexture("../images/wn.png");
	texall[1][2] = LoadTexture("../images/wb.png");
	texall[1][3] = LoadTexture("../images/wr.png");
	texall[1][4] = LoadTexture("../images/wq.png");
	texall[1][5] = LoadTexture("../images/wk.png");
	
	//starting pos assign
	startingp.w_to_move=1;
	startingp = emptyp;
	set_starting_pos(0);
	set_starting_pos(1);
	//fai or per trovare all
	startingp.all = startingp.occ[0] | startingp.occ[1];
	board_reset();
}

float scalepiece = 0.5f;

void pieces_draw(bool turn){
	Position lastp = currentp;
	for (int i=0;i<6;i++){//type of piece currently looking at
		while(lastp.pieces[turn][i]){
			int sq = pop_lsb(lastp.pieces[turn][i]);
			//draw
			int col = sq%8;
			int row = (sq - col)/8;

			Vector2 pos;
			pos.x = corner.x + squaresize*col + (float)squaresize/2.0f - texall[turn][i].width*scalepiece/2.0f;
			pos.y = corner.y + squaresize*row + (float)squaresize/2.0f - texall[turn][i].height*scalepiece/2.0f;
			
			DrawTextureEx(
				texall[turn][i],
				pos,
				0,
				scalepiece,
				WHITE
			);
		}
	}
}

void board_draw(void){

    DrawRectangle(corner.x, corner.y, chessboardsize, chessboardsize, BEIGE);

    int row;
	for(int col = 0; col < 8; col++){//uno ogni 2 quadretti
		for(int orow = 0; orow < 8; orow++){
			
			if(!settings[TURN])//giochi come il nero:devi invertire
				row = 7 - orow;
			else 
				row = orow;
			
			if(( col + orow )%2==1){
				int top=corner.y+row*squaresize;
				int left=corner.x+col*squaresize;
				DrawRectangle(left, top, squaresize, squaresize, DARKBROWN);
			}
			
		}
	}		
	
	pieces_draw(0);
	pieces_draw(1);
	
}