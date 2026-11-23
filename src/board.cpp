#include "board.h"
#include "game_main.h"
#include "menu_main.h"
#include "board_features.h"
#include "pieces.h"
#include "evaluation.h"
#include "buttons.h"
#include "search.h"


using namespace std;

Vector2 corner;
int squaresize, chessboardsize;

bool choosing;

int end_game = NO_ENDG;

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

Move emptymv;

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

void board_reset(void){
    record.clear();
	currentp = startingp;
	record.push_back(currentp);

	held_piece = EMPTY;
	held_piece_sq = NO_SQ;
	arrows.clear();

	current_legality = legality(currentp);
	choosing = 0;

	//currentp.end_game = NO_ENDG;
	
	lobby_button.state = 0;
	restart_button.state = 0;
	
	search_gen = 1;
	target_eval_val = 0.0f;
	eval_val = 0.0f;

	bot_gen = 0;
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
	startingp.lastmove.flag = NO_MOVE;
	startingp.end_game = NO_ENDG;

	set_starting_pos(0);
	set_starting_pos(1);
	//fai or per trovare all
	startingp.all = startingp.occ[0] | startingp.occ[1];
	startingp.castling=15;

	//buttons
	game_buttons_init();

	board_reset();

	emptymv.flag = NO_MOVE;

	mtx_init();
}

Move mv;

Move build_move(const Position &s, int from, int to){
	uint8_t piece = s.type_on[from], captured = s.type_on[to], promo = EMPTY, flag = NORMAL;

	bool double_push = piece == PAWN && abs(to - from) == 16;
	bool diag_to_empty = piece == PAWN && from % 8 != to % 8 && captured == EMPTY;
	bool castle = piece == KING && abs(to - from) == 2;
	//bool promotion = piece == PAWN && (to / 8 == 0 || to / 8 == 7);

	if(double_push) flag = DOUBLE_PUSH;
	else if(diag_to_empty){ flag = EN_PASSANT; captured = PAWN; }
	else if(castle) flag = CASTLE;
	else if(captured!=EMPTY) flag = CAPTURE;
	// if(promotion){
	// 	choosing = 1;
	// }
	return Move{from, to, piece, captured, promo, flag};
}

static uint8_t rights_kept(int sq){ 
	uint8_t mxv = 0;
	mxv = ~mxv;
	switch(sq){
		case A1:return mxv & ~WQ; 
		case A5:return mxv & ~(WK|WQ); 
		case A8:return mxv & ~WK; 
		case H1:return mxv & ~BQ; 
		case H5:return mxv & ~(BK|BQ); 
		case H8:return mxv & ~BK; 
		default:return mxv; 
	} 
}

void apply_move(Position &state, const Move &mv){

	if(mv.flag == NO_MOVE)
		return;

	bool turn = state.w_to_move;
	state.lastmove = mv;
	state.w_to_move = !turn;

	state.castling &= rights_kept(mv.from) & rights_kept(mv.to); //mask per modificare movimento di re e rooks:
	//6 totali: 2 torri e 1 re per colore

	clear_board_piece(state, turn, mv.from, mv.piece);
	set_board_piece(state, turn, mv.to, (mv.promo!= EMPTY)? mv.promo : mv.piece);

	if(mv.flag == EN_PASSANT) 
		clear_board_piece(state, !turn, mv.to + (turn ? -8 : 8), PAWN);
	else if(mv.flag == CASTLE){
		bool kingside = mv.to > mv.from; //long or short?
		clear_board_piece(state, turn, kingside ? mv.from+3 : mv.from-4, ROOK);
		set_board_piece(state, turn, kingside ? mv.from+1 : mv.from-1, ROOK);
	}


}

//board handle
void change_turn(const Move &new_move){
	apply_move(currentp, new_move);
	record.push_back(currentp);
	arrows.clear();

	current_legality = legality(currentp);//aggiorno la legalità

	//check for endgame
	std::vector<Move> moves;
	gen_moves(currentp, current_legality, moves);
	currentp.end_game = check_endgame(current_legality, moves, record);

	if((currentp.w_to_move == !settings[TURN]) && settings[BOT]){
		bot_asyinc(currentp, current_legality);
	}
	else if(settings[EVAL]){
		eval_asyinc(currentp);
	}

}

// void change_turn_bot(Position& state, Legality &L, const Move &new_move, const std::vector<Move>&moves){//from bot to player
// 	apply_move(state, new_move);
// 	record.push_back(state);

// 	L = legality(state);//aggiorno la legalità

// 	state.end_game = check_endgame(L, moves);

// 	if(settings[EVAL]){
// 		eval_asyinc(currentp);
// 	}
// }

float transparency = 0.2f;

void draw_text(Vector2 position, string text, int fontsize){
	

	Vector2 padding = {chessboardsize/6.0f, chessboardsize/16.0f};
	Vector2 text_size = MeasureTextEx(text_font, text.c_str(), fontsize, 0);
    Vector2 text_position = {position.x - text_size.x/2, position.y - text_size.y/2};
	padding = Vector2Add(padding, text_size);
	DrawRectangle(position.x - padding.x/2.0f , position.y - padding.y/2.0f, padding.x, padding.y, Fade(BEIGE, 0.9f));

    //Color text_draw_color = ColorBrightness(textcolor, brightness_factor);
    DrawTextEx(text_font, text.c_str(), text_position, fontsize, 0, BLACK);
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


	//last move 
	
	if(currentp.lastmove.flag != NO_MOVE){
		Vector2 pos = sq_to_coord(currentp.lastmove.from);

		if(!settings[TURN]){
			pos = invert_coord(pos);
		}
		int top=corner.y+pos.y*squaresize;
		int left=corner.x+pos.x*squaresize;

		DrawRectangle(left, top, squaresize, squaresize, Fade(YELLOW, transparency));

		pos = sq_to_coord(currentp.lastmove.to);

		if(!settings[TURN]){
			pos = invert_coord(pos);
		}
		top=corner.y+pos.y*squaresize;
		left=corner.x+pos.x*squaresize;

		DrawRectangle(left, top, squaresize, squaresize, Fade(YELLOW, transparency));

	}
	
	pieces_draw(0);
	pieces_draw(1);

	
	if(choosing){
		//roba
		int promosq = mv.to;//square di dove si trova: da qua shifto verso alto o basso
		bool turn = currentp.w_to_move;

		int shift_constant = (turn)? -8:8;

		int sq;
		for(uint8_t p=KNIGHT; p<=QUEEN; p++){
			sq = (promosq + (p-1)*shift_constant);


			Vector2 pos = sq_to_coord(sq);

			if(!settings[TURN])
				pos = invert_coord(pos);

			int top=corner.y+pos.y*squaresize;
			int left=corner.x+pos.x*squaresize;
			DrawRectangle(left, top, squaresize, squaresize, (turn)?WHITE:BLACK);

			pos.x = left + (float)squaresize/2.0f - texall[turn][p].width*scalepiece/2.0f;
			pos.y = top + (float)squaresize/2.0f - texall[turn][p].height*scalepiece/2.0f;
			draw_piece(turn, p, pos);
		}

	}

	draw_features();

	if(currentp.end_game != NO_ENDG){
		//disegno schermata finale
		DrawRectangle(corner.x, corner.y, chessboardsize, chessboardsize, Fade(RED, transparency));

		//disegno scritta di endgame
		//endgame_button.text
		string text;
		if(currentp.end_game == CHECKMATE){
			text = (currentp.w_to_move)? "BLACK WINS" : "WHITE WINS";
		}
		else{
			text =  "TIE";
		}

		Vector2 position = (Vector2){corner.x + chessboardsize/2.0f, corner.y + chessboardsize/2.0f};
		
		int f_size = 90;
		draw_text(position, text, f_size);


	}
}