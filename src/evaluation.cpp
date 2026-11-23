#include "evaluation.h"
#include "board.h"
#include "pieces.h"
#include "board_features.h"
#include "menu_main.h"


float eval_val = 0.0f;
std::atomic<float> target_eval_val{0.0f};
float eval_speed = 5.0f;

void gen_moves(const Position& state, const Legality& L, std::vector<Move>& moves){
	bool turn = state.w_to_move;
	for(uint8_t piece = PAWN; piece<=KING; piece++){
		uint64_t board = state.pieces[turn][piece];
		while(board){
			int from_sq = pop_lsb(board);
			uint64_t to_bb = find_moves(state, L, piece, from_sq);
			while(to_bb){
				int to_sq = pop_lsb(to_bb);
				Move move = build_move(state, from_sq, to_sq);
				if(piece == PAWN && (to_sq/8 == 0 || to_sq/8 == 7)){//promotion
					for(uint8_t promo_p = KNIGHT; promo_p<=QUEEN; promo_p ++){
						move.promo = promo_p;
						moves.push_back(move);
					}
				}
				else
					moves.push_back(move);
			}
		}
	}	
}

static bool same_pos(const Position &a, const Position &b){
	return a.w_to_move == b.w_to_move && a.castling == b.castling
	    && memcmp(a.pieces, b.pieces, sizeof(a.pieces)) == 0;
}

const uint64_t LIGHT = 0x55AA55AA55AA55AAULL;//light squares

bool insufficient_material(const Position &state){
	for(int c = 0; c < 2; c++)
		if(state.pieces[c][PAWN] | state.pieces[c][ROOK] | state.pieces[c][QUEEN]) return false;
	
	uint64_t N = state.pieces[0][KNIGHT] | state.pieces[1][KNIGHT];
	uint64_t B = state.pieces[0][BISHOP] | state.pieces[1][BISHOP];

	int minors = __builtin_popcountll(N | B);
	if(minors <= 1) return true;

	if(!N && state.pieces[0][BISHOP] && state.pieces[1][BISHOP] && minors == 2){
		return !!(state.pieces[0][BISHOP] & LIGHT) == !!(state.pieces[1][BISHOP] & LIGHT);//!! so that it becomes a bool: bishops on same color
	}//1 B vs 1 B of same color
	
	//to add other cases


	return false;
}


int check_endgame(const Legality& L, const std::vector<Move>& moves, const std::vector<Position>& hist){
	//0 = non finisce, 1 = tie, 2 = checkmate per il colore opposto (!w_to_move)

	if(moves.empty()){
		int n = __builtin_popcountll(L.checkers);
		if(n==0)
			return TIE;
		else if(n!=0)//stalemate
			return CHECKMATE;
	}

	int contam = 1;

	for(int i =0;i<hist.size()-1; i++){
		if(same_pos(hist[i], hist[hist.size()-1])){
			contam++;
			if(contam>=3)
				return TIE;//repetition
		}
			
	}
	//tie by insufficient material:
	//hist[hist.size()-1] is current
	if(insufficient_material(hist[hist.size()-1]))
		return TIE;

	return NO_ENDG;

}

enum GAME_STAGE: int{
    MID_GAME, END_GAME
};

using Mtx8x8 = std::array<std::array<float, 8>, 8>;;//colonna, riga

Mtx8x8 pawn_mtx_mg={{
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	13, 	15,		15,		13,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0}
}};

Mtx8x8 pawn_mtx_eg={{
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	13, 	15,		15,		13,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0}
}};

Mtx8x8 knight={{
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	13, 	15,		15,		13,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0}
}};

Mtx8x8 bishop={{
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	13, 	15,		15,		13,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0}
}};

Mtx8x8 rook_mg={{
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	13, 	15,		15,		13,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0}
}};

Mtx8x8 rook_eg={{
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	13, 	15,		15,		13,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0}
}};

Mtx8x8 queen_mg={{
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	13, 	15,		15,		13,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0}
}};


Mtx8x8 queen_eg={{
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	13, 	15,		15,		13,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0}
}};

Mtx8x8 king_mg={{
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	13, 	15,		15,		13,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0}
}};

Mtx8x8 king_eg={{
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	13, 	15,		15,		13,		0,		0},
	{10, 	10, 	10, 	10,		10,		10,		10,		10},
	{0, 	0,  	 0, 	0,		0,		0,		0,		0}
}};
/*
Mtx8x8 invert_mtx(Mtx8x8 mtx){//mtxs are for white, for black i have to invert it
	Mtx8x8 newmtx;
	for(int i=0; i<8; i++){
		for(int j=0; j<8; j++){
			newmtx[i][j] = mtx[7-i][7-j];
		}
	}
	return newmtx;
}
	*/
std::array<std::array<Mtx8x8,2>,6>pos_mtx;//da gestire anche conversione mid game - end game

void mtx_init(void){
	for(uint8_t piece = PAWN; piece<=KING; piece++){

		Mtx8x8* mtx_mg = &pos_mtx[piece][MID_GAME];
		Mtx8x8* mtx_eg = &pos_mtx[piece][END_GAME];

		switch (piece){
			case PAWN:
				*mtx_mg = pawn_mtx_mg;
				*mtx_eg = pawn_mtx_eg;
			case KNIGHT:
				*mtx_mg = knight;
				*mtx_eg = knight;
			case BISHOP:
				*mtx_mg = bishop;
				*mtx_eg = bishop;
			case ROOK:
				*mtx_mg = rook_mg;
				*mtx_eg = rook_eg;
			case QUEEN:
				*mtx_mg = queen_mg;
				*mtx_eg = queen_eg;
			case KING:
				*mtx_mg = king_mg;
				*mtx_eg = king_eg;
		}
		// pos_mtx[0][piece][MID_GAME] = invert_mtx(pos_mtx[1][piece][MID_GAME]);
		// pos_mtx[0][piece][END_GAME] = invert_mtx(pos_mtx[1][piece][END_GAME]);
	}
}

int max_npieces = 32;

float pos_value(int npieces, float mid_game, float end_game){//media pesata

	float start_weight = (float)npieces;
	float end_wheight = ((float)max_npieces - (float)npieces);
	
	return ((mid_game)*start_weight + (end_game)*end_wheight)/(float)max_npieces;
}

float evaluate_pos(Position& state){
	
    float ris = 0.0f;
	int sq;//per ora è solo valori: poi anche valori in base alla posizione
	//MAPPE DI CONVENIENZA -> devo capire come farle (? da file forse)
	float val;
	float val_pos;
	//conversion mid game -> end game idea:
	float mid_game, end_game;
	int npieces = __builtin_popcountll(state.all);

    for(uint8_t piece = PAWN; piece<=KING; piece++){
		uint64_t board = state.pieces[0][piece];
        while(board){
            val =piece_value[piece];

            sq = pop_lsb(board);
			Vector2 pos=sq_to_coord(sq);
			pos = invert_coord(pos);

			mid_game = pos_mtx[piece][MID_GAME][pos.x][pos.y]/100.0f;
			end_game = pos_mtx[piece][END_GAME][pos.x][pos.y]/100.0f;
			val_pos = pos_value(npieces, mid_game, end_game);
			val += val_pos;//position value

			ris -= val;
        }
		board = state.pieces[1][piece];
        while(board){
            val=piece_value[piece];

            sq = pop_lsb(board);
			Vector2 pos=sq_to_coord(sq);

			mid_game = pos_mtx[piece][MID_GAME][pos.x][pos.y]/100.0f;
			end_game = pos_mtx[piece][END_GAME][pos.x][pos.y]/100.0f;
			val_pos = pos_value(npieces, mid_game, end_game);
			val += val_pos;//position value

			ris+=val;
        }
	}
	std::cout<<ris<<std::endl;
    return ris;
}




void eval_bar_draw(){
	
	float k = 8;

	
	float barheight = chessboardsize;
	float barwidth = squaresize/2;
	//lo spostamento dovrebbe essere a livello logaritmico es log(|val|) o boh, comunque su un totale di 100 - 100
	eval_val += (target_eval_val - eval_val) * eval_speed * GetFrameTime();//si avvicina a target_eval_val
	
	if(fabs(target_eval_val - eval_val) < 0.05f)
		eval_val = target_eval_val;
	
	float barprop;// tra -1 e +1, indica quanto verso un lato va la riga
	if(fabs(eval_val)>1000.0f)//se è matto praticamente (capisco come gestirlo poi) !!!
		barprop = (eval_val > 0)? 1.0f:-1.0f;
	else 
		barprop = tanh(eval_val / k);
	
	float pixelsplit = (barprop + 1.0f)/2.0f; //praticamente altezza a cui fare sta cosa : la moltiplichi * barheight e hai 
	
	float whiteheight = barheight - pixelsplit*barheight;//quanto devo andare giu
	
	Vector2 barpos;
	barpos.x = corner.x - barwidth - squaresize/5;
	barpos.y = corner.y;

	DrawRectangle(barpos.x, barpos.y, barwidth, barheight, DARKGRAY);
	
	if(pixelsplit != 0)DrawRectangle(barpos.x, barpos.y + whiteheight, barwidth, pixelsplit*barheight, WHITE);
	
}