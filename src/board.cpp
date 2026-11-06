#include "board.h"
#include "game_main.h"
#include "menu_main.h"
#include "board_features.h"
#include "pieces.h"

using namespace std;

Vector2 corner;
int squaresize, chessboardsize;

//BIT OPERATIONS ON BOARD
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

inline bool is_occ(U64 bb, int sq) { //checks if square number sq is occupied in board bb
	return bb >> sq & 1; 
}
inline void set_bit(U64& bb, int sq)   { bb |= 1ULL << sq; }
inline void clear_bit(U64& bb, int sq) { bb &= ~(1ULL << sq); }

//BOARD CHANGE OPERATIONS
void set_board_piece(Position &board, bool color, int sq, int piece){
	set_bit(board.pieces[color][piece],sq);
	set_bit(board.occ[color],sq);
	set_bit(board.all,sq);
}

void clear_board_piece(Position &board, bool color, int sq, int piece){
	clear_bit(board.pieces[color][piece],sq);
	clear_bit(board.occ[color],sq);
	clear_bit(board.all,sq);
}

//COORDINATES OPERATION
bool on_board(Vector2 pos){
    //checks if the position is on the chessboard
    return (pos.x >= corner.x && pos.x <= corner.x + chessboardsize && pos.y >= corner.y && pos.y <= corner.y + chessboardsize);
}
int invert_onb(int sq){//invert on board
	//devo invertire da un lato all'altro: A1 diventa H1 e viceversa
	int res,ris;
	res = sq%8;
	ris = 56-sq+2*res;
	return ris;
}
Vector2 invert_coord(Vector2 coord){
	coord.x = 7-coord.x;
	coord.y = 7-coord.y;
	return coord;
}
int coord_to_sq(Vector2 coord){
	int row = coord.y;
	int col = coord.x;
	//roba
	//coords 0 =
	int sq = row*8 + col;
	return sq;
}

Vector2 sq_to_coord(int sq){
	int col = sq%8;
	
	int row = (sq - col)/8;
	return (Vector2){(float)col,(float)row};
}

Vector2 chessboard_pos(Vector2 abs_pos){
	Vector2 chesspos = Vector2Subtract(abs_pos, corner);//chessboard pos: quanti pixel dal bordo
	
	int row = floor(((float)chesspos.y)/(float)squaresize);//da 0 a 7
	int col = floor(((float)chesspos.x)/(float)squaresize);
	return Vector2{(float)col,(float)row};
}

/*reminder: bitwise operations
& = and
| = or
~ = not
^ = xor
*/

vector<Position>record;

Position startingp;
Position currentp;
Position emptyp;

void board_reset(void){
    record.clear();
	arrows.clear();
	//record.push_back(startingp);
	currentp = startingp;

	held_piece = EMPTY;
	held_piece_sq = NO_SQ;
	arrows.clear();
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
	for(int i=B1;i<=B8;i++){//pawns
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

	//queen
	piece = QUEEN;
	sq = A4;//perche mi mette il re su a4?
	sq = (color)?sq:invert_onb(sq);
	set_board_piece(startingp, color, sq, piece);

	//king
	piece = KING;
	sq = A5;
	sq = (color)?sq:invert_onb(sq);
	set_board_piece(startingp, color, sq, piece);
}

void board_init(void){
    corner.y=(ScreenHeight)/9;
	squaresize=corner.y;
	corner.y/=2;
	chessboardsize=squaresize*8;
	corner.x = (ScreenWidth - chessboardsize ) - corner.y;
	

	load_pieces_textures();
	//starting pos assign
	startingp.w_to_move=1;
	startingp = emptyp;
	set_starting_pos(0);
	set_starting_pos(1);
	//fai or per trovare all
	startingp.all = startingp.occ[0] | startingp.occ[1];

	
	board_reset();
}


//board handle


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
	
	draw_features();
}