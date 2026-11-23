#include "evaluation.h"
#include "board.h"
#include "pieces.h"
#include "board_features.h"
#include "menu_main.h"
#include "search.h"

int max_depth = 5;//7
int max_capture_depth = 15; //8 captures after ->15

std::atomic<int>search_gen{0};



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


constexpr float INF = 1.0e8 + 7;
constexpr float MATE = 1.0e6 + 7;

float try_moves(int depth, Position state, const Move &mv, std::vector<Position>& hist, float alpha, float beta, int id){
	//std::cout<<depth<<" "<<id<<std::endl;
	if(id!=0 && (id != search_gen))
		return 0.0f;//non serve piu

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
	std::thread([state, id, hist=record]() mutable{
		float alpha = -INF;
		float beta = INF;
		float r = try_moves(0, state, emptymv, hist, alpha, beta, id);
		if(id == search_gen)target_eval_val = r;
		std::cout<<" eval: "<<id<<" - "<< search_gen<< " - ris: "<<r<<std::endl;
	}).detach();
}

std::atomic<bool> bot_ready{false};
std::atomic<int> bot_gen;
Move bot_move;

void do_best_move(const Position& state,const Legality& leg, std::vector<Position>&hist, int id){
	
	Move ris = emptymv;

	bool (*better)(float, float) = (!settings[TURN]) 
		? +[](float a, float b) { return a > b; }//maximize for white
		: +[](float a, float b) { return a < b; };//minimize for black

	std::vector<Move>bot_moves;
	gen_moves(state, leg, bot_moves);
	sort_moves(bot_moves);

	float best;
	best = (state.w_to_move)? -INF:INF;

	float alpha = -INF;
	float beta = INF;
	float* pivot = (state.w_to_move)? &alpha:&beta;//what to update

	for(Move& move: bot_moves){
		if(id != search_gen)
			return;
		
		float val_move = try_moves(1, state, move, hist, alpha, beta, id);//i use search gen to stop it all toghether and not divide search gen - bot gen

		if(better(val_move, best)){
			best = val_move;
			ris = move;
		}
		if(better(best, *pivot)){
			*pivot = best;
		}
		//ris = best(ris, val_move);
		if(alpha>=beta)break;
	}

	if(id == search_gen){
		bot_move = ris;
		bot_ready = true;

		if(settings[EVAL]) 
			target_eval_val = best;
	}

	
	//change_turn_bot(state, leg, ris, bot_moves);
}

void bot_asyinc(const Position& state, const Legality &leg){
	int id = ++search_gen;
	std::thread([state, leg, id, hist = record]() mutable{
		bot_ready = false;
		bot_move = emptymv;
		
		do_best_move(state, leg, hist, id);
	}).detach();
}