#include "evaluation.h"
#include "board.h"
#include "pieces.h"
#include "board_features.h"
#include "menu_main.h"

int max_depth = 4;
int max_capture_depth = 7; //10 captures

float eval_val = 0.0f;
std::atomic<float> target_eval_val{0.0f};
float eval_speed = 1.0f;

std::atomic<int>search_gen{1};

void sort_moves(std::vector<Move>& moves){
	//for alpha beta pruning
	auto score = [](const Move& mv){
		float s = 0.0f;
		if(mv.captured != EMPTY) s += 100.0f + 10.0f*piece_value[mv.captured] - piece_value[mv.piece]; // vittima grossa, attaccante piccolo
		if(mv.promo    != EMPTY) s += 80.0f + piece_value[mv.promo];
		return s;
	};

	std::stable_sort(moves.begin(), moves.end(), 
		[&](const Move& a, const Move& b){ 
			return score(a) > score(b); 
	});
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


constexpr float INF = 1.0e8 + 7;
constexpr float MATE = 1.0e6 + 7;

float try_moves(int depth, Position state, const Move &mv, std::vector<Position>& hist, float alpha, float beta, int id){
	//std::cout<<depth<<" "<<id<<std::endl;
	if(id!=0 && (id != search_gen))return 0.0f;//non serve piu

	if(mv.flag != NO_MOVE)
		apply_move(state, mv);

	if((depth>=max_depth && mv.flag!=CAPTURE) || depth>= max_capture_depth){
		return evaluate_pos(state);
	}

	Legality hypot_legality = legality(state);//hypotethical
	std::vector<Move>moves;
	gen_moves(state, hypot_legality, moves);

	hist.push_back(state);//if all is ok i push back for checking stuff

	state.end_game = check_endgame(hypot_legality, moves, hist);
	

	float ris=(state.w_to_move)? -INF: INF;//worst case
	
	if(state.end_game != NO_ENDG){
		hist.pop_back();
		return (state.end_game == TIE)?0.0f:(state.w_to_move)? -(MATE-depth) : (MATE-depth);//checkmate = worst case
	}

	

	//float (*best)(float a, float b);
	//best = (state.w_to_move)?std::fmaxf:std::fminf;
	sort_moves(moves);
	bool checked = 0;
	for(Move& move: moves){
		if(id!=0 && (id != search_gen))
			break;//stop searching

		if((depth>max_depth && move.flag!=CAPTURE))//se sono gia a max_depth e non è cattura ignoro
			continue;
		
		checked = 1;

		float val_move = try_moves(depth+1, state, move, hist, alpha, beta, id);

		if(state.w_to_move){
			ris = std::max(ris, val_move);
			alpha = std::max(ris, alpha);
		}
		else {
			ris = std::min(ris, val_move);
			beta = std::min(ris, beta);
		}
		//ris = best(ris, val_move);
		if(alpha>=beta)
			break;
	}

	if(!checked)
		ris = evaluate_pos(state);
	hist.pop_back();
	//state = record[record.size()-1];//sistemo lo stato
	
	return ris;

}

void eval_asyinc(Position& state){
	int id = ++search_gen;
	std::thread([state, id, hist=record] mutable{
		float alpha = -INF;
		float beta = INF;
		float r = try_moves(0, state, emptymv, hist, alpha, beta, id);
		if(id == search_gen)target_eval_val = r;
		std::cout<<id<<" - "<< search_gen<< " - ris: "<<r<<std::endl;
	}).detach();
}

std::atomic<bool> bot_ready{false};
std::atomic<int> bot_gen;
Move bot_move;

void do_best_move(const Position& state,const Legality& leg, std::vector<Position>&hist, int id){
	
	Move ris = emptymv;

	bool (*better)(float, float) = (!settings[TURN]) 
		? +[](float a, float b) { return a > b; }   // Maximize (e.g., White)
		: +[](float a, float b) { return a < b; };  // Minimize (e.g., Black)

	std::vector<Move>bot_moves;
	gen_moves(state, leg, bot_moves);
	sort_moves(bot_moves);

	float best;
	best = (state.w_to_move)? -INF:INF;

	float alpha = -INF;
	float beta = INF;
	float* pivotcheck = (state.w_to_move)? &alpha:&beta;//what to update

	for(Move& move: bot_moves){
		if(id != bot_gen)
			return;
		
		float val_move = try_moves(1, state, move, hist, alpha, beta, id);

		if(better(val_move, best)){
			best = val_move;
			ris = move;
		}
		if(better(best, *pivotcheck)){
			*pivotcheck = best;
		}
		//ris = best(ris, val_move);
		if(alpha>=beta)break;
	}

	if(id == bot_gen){
		bot_move = ris;
		bot_ready = true;
	}

	
	//change_turn_bot(state, leg, ris, bot_moves);
}

void bot_asyinc(const Position& state, const Legality &leg){
	int id = ++bot_gen;
	std::thread([state, leg, id, hist = record] mutable{
		bot_ready = false;
		bot_move = emptymv;
		do_best_move(state, leg, hist, id);
	}).detach();
}


void eval_bar_draw(){
	
	
	//float k = 8;

	float barheight = chessboardsize;
	float barwidth = squaresize/2;
	//lo spostamento dovrebbe essere a livello logaritmico es log(|val|) o boh, comunque su un totale di 100 - 100
	// eval_val += (target_eval_val - eval_val) * eval_speed * GetFrameTime();//si avvicina a target_eval_val
	
	// if(fabs(target_eval_val - eval_val) < 0.05f)
	// 	eval_val = target_eval_val;
	
	// float barprop;// tra -1 e +1, indica quanto verso un lato va la riga
	// if(fabs(eval_val)>1000.0f)//se è matto praticamente (capisco come gestirlo poi) !!!
	// 	barprop = (eval_val > 0)? 1.0f:-1.0f;
	// else 
		//barprop = tanh(eval_val / k);

	const float k = 5.4f;   // tanh(x/5.4) ≈ curva win% Lichess: +1 pedone 59%, +3 75%, +9 96%
	eval_val += (tanhf(target_eval_val / k) - eval_val) * (1.0f - expf(-eval_speed * GetFrameTime()));
	float barprop = eval_val;   // ora è la frazione -1..+1
	
	float pixelsplit = (barprop + 1.0f)/2.0f; //praticamente altezza a cui fare sta cosa : la moltiplichi * barheight e hai 
	
	float whiteheight = barheight - pixelsplit*barheight;//quanto devo andare giu
	
	Vector2 barpos;
	barpos.x = corner.x - barwidth - squaresize/5;
	barpos.y = corner.y;

	DrawRectangle(barpos.x, barpos.y, barwidth, barheight, DARKGRAY);
	
	if(pixelsplit != 0)DrawRectangle(barpos.x, barpos.y + whiteheight, barwidth, pixelsplit*barheight, WHITE);
	
}