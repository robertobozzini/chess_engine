#include "board.h"
#include "game_main.h"
#include "menu_main.h"
#include "board_features.h"
#include "pieces.h"

using namespace std;

Vector2 corner;
int squaresize, chessboardsize;



//BOARD CHANGE OPERATIONS

void clear_board_piece(Position &board, bool color, int sq, int piece){
	clear_bit(board.pieces[color][piece], sq);
	clear_bit(board.occ[color], sq);
	clear_bit(board.all, sq);
	board.type_on[sq] = EMPTY;
}

void set_board_piece(Position &board, bool color, int sq, int piece){
	if(board.type_on[sq]!=EMPTY){
		clear_board_piece(board, !color, sq, board.type_on[sq]);
	}
	
	set_bit(board.pieces[color][piece], sq);
	set_bit(board.occ[color], sq);
	set_bit(board.all, sq);
	board.type_on[sq] = piece;
}

//COORDINATES OPERATION
bool on_board(Vector2 pos){
    //checks if the position is on the chessboard
    return (pos.x >= corner.x && pos.x <= corner.x + chessboardsize && pos.y >= corner.y && pos.y <= corner.y + chessboardsize);
}
bool in_board(Vector2 coord){
	return (coord.x>=0 && coord.x<8 && coord.y>=0 && coord.y<8);
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
int coord_to_sq(Vector2 coord){//qualcosa di sbagliato
	int row = coord.y;
	int col = coord.x;
	//roba
	//coords 0 =
	int sq = (7-row)*8 + col;
	return sq;
}

Vector2 sq_to_coord(int sq){
	int col = sq%8;
	
	int row = 7-(sq - col)/8;
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
	currentp = startingp;
	record.push_back(currentp);

	held_piece = EMPTY;
	held_piece_sq = NO_SQ;
	arrows.clear();
}



void set_starting_pos(bool color){
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
	att_tables_init();
	
	//starting pos assign
	for(int i=0;i<64;i++){
		emptyp.type_on[i]=EMPTY;
	}
	startingp = emptyp;
	startingp.w_to_move=1;

	set_starting_pos(0);
	set_starting_pos(1);
	//fai or per trovare all
	startingp.all = startingp.occ[0] | startingp.occ[1];

	board_reset();
}


//board handle
void change_turn(void){
	currentp.w_to_move = !currentp.w_to_move;
	record.push_back(currentp);
	arrows.clear();
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
	
	draw_features();
}