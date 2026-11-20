#include "evaluation.h"
#include "board.h"
#include "pieces.h"
#include "board_features.h"

int max_depth = 4;
int max_capture_depth = 10; //10 captures

float eval_val = 0.0f;
float target_eval_val = 0.0f;
float eval_speed = 1.0f;

void sort_moves(std::vector<Move>& moves){
	//for alpha beta pruning
	return;
}

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

int check_endgame(const Legality& L, const std::vector<Move>& moves){
	//0 = non finisce, 1 = tie, 2 = checkmate per il colore opposto (!w_to_move)

	//manca patta per ripetizione
	if(moves.empty()){
		int n = __builtin_popcountll(L.checkers);
		if(n==0)
			return TIE;
		else if(n!=0)//stalemate
			return CHECKMATE;
	}

	int contam = 1;
	for(int i =0;i<record.size()-1; i++){
		if(same_pos(record[i], record[record.size()-1])){
			contam++;
			if(contam>=3)
				return TIE;//repetition
		}
			
	}
	return NO_ENDG;

}

float evaluate_pos(Position& state){
	
    float ris = 0.0f;
	int sq;//per ora è solo valori: poi anche valori in base alla posizione
	//MAPPE DI CONVENIENZA -> devo capire come farle (? da file forse)
	float val;
    for(uint8_t piece = PAWN; piece<=KING; piece++){
		uint64_t board = state.pieces[0][piece];
        while(board){
            val =piece_value[piece];
            sq = pop_lsb(board);

			ris -= val;
        }
		board = state.pieces[1][piece];
        while(board){
            val=piece_value[piece];
            sq = pop_lsb(board);

			ris+=val;
        }
	}

    return ris;
}


constexpr float INF = 1.0e9 + 7;

float try_moves(int depth, Position state, const Move &mv){
	Legality hypot_legality;//hypotethical
	std::vector<Move>moves;
	if(mv.flag != NO_MOVE)
		apply_move(state, mv);
	hypot_legality = legality(state);
	gen_moves(state, hypot_legality, moves);

	state.end_game = check_endgame(hypot_legality, moves);
	float ris=(state.w_to_move)? -INF: INF;

	if(state.end_game != NO_ENDG){
		ris = (state.end_game == TIE)?0.0f:ris;
		record.pop_back();
		state = record[record.size()-1];
		return ris;
	}
	else if((depth>=max_depth && mv.flag!=CAPTURE) || depth>= max_capture_depth){
		ris = evaluate_pos(state);
		record.pop_back();
		state = record[record.size()-1];
		return ris;
	}

	float (*best)(float a, float b);
	best = (state.w_to_move)?std::fmaxf:std::fminf;

	for(Move& move: moves){
		if((depth>max_depth && move.flag!=CAPTURE))//se sono gia a max_depth
			continue;
		ris = best(ris, try_moves(depth+1, state, move));
	}
	record.pop_back();
	// record.pop_back();
	// state = record[record.size()-1];//sistemo lo stato
	
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