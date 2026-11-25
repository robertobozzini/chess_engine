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

	if(!N) return !((B & LIGHT) && (B & ~LIGHT)); 
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
	int cur_n = __builtin_popcountll(hist[hist.size()-1].all);

	for(int i =0;i<hist.size()-1; i++){
		if(__builtin_popcountll(hist[i].all) != cur_n) break;
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
	{0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f},
	{0.50f, 	0.50f, 	0.50f, 	0.50f, 	0.50f, 	0.50f, 	0.50f, 	0.50f},
	{0.10f, 	0.10f, 	0.20f, 	0.30f, 	0.30f, 	0.20f, 	0.10f, 	0.10f},
	{0.05f, 	0.05f, 	0.15f, 	0.25f, 	0.25f, 	0.15f, 	0.05f, 	0.05f},
	{0.00f, 	0.00f, 	0.10f, 	0.20f, 	0.20f, 	0.10f, 	0.00f, 	0.00f},
	{0.05f, 	-0.05f, -0.05f,  0.10f, 0.10f, 	-0.05f, -0.05f, 0.05f},
	{0.05f, 	0.10f, 	0.00f, 	-0.20f, -0.20f, 0.00f, 	0.10f, 	0.05f},
	{0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f}
}};

Mtx8x8 pawn_mtx_eg={{
	{0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f},
	{0.65f, 	0.65f, 	0.65f, 	0.65f, 	0.65f, 	0.65f, 	0.65f, 	0.65f},
	{0.50f, 	0.50f, 	0.50f, 	0.50f, 	0.50f, 	0.50f, 	0.50f, 	0.50f},
	{0.35f, 	0.35f, 	0.35f, 	0.35f, 	0.35f, 	0.35f, 	0.35f, 	0.35f},
	{0.20f, 	0.20f, 	0.20f, 	0.20f, 	0.20f, 	0.20f, 	0.20f, 	0.20f},
	{0.10f, 	0.10f, 	0.10f, 	0.10f, 	0.10f, 	0.10f, 	0.10f, 	0.10f},
	{0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f},
	{0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f}
}};

Mtx8x8 knight={{
	{-0.50f, 	-0.40f, -0.30f, -0.30f, -0.30f, -0.30f, -0.40f, -0.50f},
	{-0.40f, 	-0.20f, 0.00f, 	0.00f, 	0.00f, 	0.00f, -0.20f, -0.40f},
	{-0.30f, 	0.00f, 	0.10f, 	0.15f, 	0.15f, 	0.10f, 	0.00f, 	-0.30f},
	{-0.30f, 	0.05f, 	0.15f, 	0.20f, 	0.20f, 	0.15f, 	0.05f, 	-0.30f},
	{-0.30f, 	0.00f, 	0.15f, 	0.20f, 	0.20f, 	0.15f, 	0.00f, 	-0.30f},
	{-0.30f, 	0.05f, 	0.10f, 	0.15f, 	0.15f, 	0.10f, 	0.05f, 	-0.30f},
	{-0.40f, 	-0.20f, 0.00f, 	0.05f, 	0.05f, 	0.00f, 	-0.20f, -0.40f},
	{-0.50f, 	-0.40f, -0.30f, -0.30f, -0.30f, -0.30f, -0.40f, -0.50f}
}};

Mtx8x8 bishop={{
	{-0.20f, 	-0.10f, 	-0.10f, 	-0.10f, 	-0.10f, 	-0.10f, 	-0.10f, 	-0.20f},
	{-0.10f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	-0.10f},
	{-0.10f, 	0.00f, 	0.05f, 	0.10f, 	0.10f, 	0.05f, 	0.00f, 	-0.10f},
	{-0.10f, 	0.05f, 	0.05f, 	0.10f, 	0.10f, 	0.05f, 	0.05f, 	-0.10f},
	{-0.10f, 	0.00f, 	0.10f, 	0.10f, 	0.10f, 	0.10f, 	0.00f, 	-0.10f},
	{-0.10f, 	0.10f, 	0.10f, 	0.10f, 	0.10f, 	0.10f, 	0.10f, 	-0.10f},
	{-0.10f, 	0.05f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.05f, 	-0.10f},
	{-0.20f, 	-0.10f, 	-0.20f, 	-0.10f, 	-0.10f, 	-0.20f, 	-0.10f, 	-0.20f}
}};

Mtx8x8 rook_mg={{
	{0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f},
	{0.05f, 	0.10f, 	0.10f, 	0.10f, 	0.10f, 	0.10f, 	0.10f, 	0.05f},
	{-0.05f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	-0.05f},
	{-0.05f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	-0.05f},
	{-0.05f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	-0.05f},
	{-0.05f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	-0.05f},
	{-0.05f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	-0.05f},
	{0.00f, 	0.00f, 	0.00f, 	0.05f, 	0.05f, 	0.00f, 	0.00f, 	0.00f}
}};

Mtx8x8 rook_eg={{
	{0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f},
	{0.10f, 	0.10f, 	0.10f, 	0.10f, 	0.10f, 	0.10f, 	0.10f, 	0.10f},
	{0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f},
	{0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f},
	{0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f},
	{0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f},
	{0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f},
	{0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f}
}};

Mtx8x8 queen_mg={{
	{-0.20f, 	-0.10f, -0.10f, -0.05f, -0.05f, -0.10f, -0.10f, -0.20f},
	{-0.10f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	-0.10f},
	{-0.10f, 	0.00f, 	0.05f, 	0.05f, 	0.05f, 	0.05f, 	0.00f, 	-0.10f},
	{-0.05f, 	0.00f, 	0.05f, 	0.05f, 	0.05f, 	0.05f, 	0.00f, 	-0.05f},
	{-0.10f, 	0.00f, 	0.05f, 	0.05f, 	0.05f, 	0.05f, 	0.00f, 	-0.10f},
	{-0.15f, 	0.00f, 	0.05f, 	0.05f, 	0.05f, 	0.05f, 	0.00f, 	-0.15f},
	{-0.10f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	-0.10f},
	{-0.25f, 	-0.15f, -0.10f, -0.05f, -0.05f, -0.10f, -0.15f,	-0.25f}
}};

Mtx8x8 queen_eg={{
	{-0.20f, 	-0.10f, -0.10f, -0.05f, -0.05f, -0.10f, -0.10f, -0.20f},
	{-0.10f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	-0.10f},
	{-0.10f, 	0.00f, 	0.05f, 	0.05f, 	0.05f, 	0.05f, 	0.00f, 	-0.10f},
	{-0.05f, 	0.00f, 	0.05f, 	0.05f, 	0.05f, 	0.05f, 	0.00f, 	-0.05f},
	{-0.05f, 	0.00f, 	0.05f, 	0.05f, 	0.05f, 	0.05f, 	0.00f, 	-0.05f},
	{-0.05f, 	0.00f, 	0.05f, 	0.05f, 	0.05f, 	0.05f, 	0.00f, 	-0.05f},
	{-0.10f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	-0.10f},
	{-0.15f, 	-0.10f, -0.10f, -0.05f, -0.05f, -0.10f, -0.10f,	-0.15f}
}};

Mtx8x8 king_mg={{
	{-0.30f, 	-0.40f, 	-0.40f, 	-0.50f, 	-0.50f, 	-0.40f, 	-0.40f, 	-0.30f},
	{-0.30f, 	-0.40f, 	-0.40f, 	-0.50f, 	-0.50f, 	-0.40f, 	-0.40f, 	-0.30f},
	{-0.30f, 	-0.40f, 	-0.40f, 	-0.50f, 	-0.50f, 	-0.40f, 	-0.40f, 	-0.30f},
	{-0.30f, 	-0.40f, 	-0.40f, 	-0.50f, 	-0.50f, 	-0.40f, 	-0.40f, 	-0.30f},
	{-0.20f, 	-0.30f, 	-0.30f, 	-0.40f, 	-0.40f, 	-0.30f, 	-0.30f, 	-0.20f},
	{-0.10f, 	-0.20f, 	-0.20f, 	-0.20f, 	-0.20f, 	-0.20f, 	-0.20f, 	-0.10f},
	{0.20f, 	0.20f,		-0.10f, 	-0.20f, 	-0.20f, 	-0.10f, 	0.20f, 		0.20f},
	{0.10f, 	0.40f, 		0.20f, 		-0.10f, 	0.00f, 		-0.10f, 	0.60f, 		0.30f}
}};

Mtx8x8 king_eg={{
	{-0.50f, 	-0.40f, 	-0.30f, -0.20f, -0.20f, -0.30f, -0.40f, 	-0.50f},
	{-0.30f, 	-0.20f, 	-0.10f, 0.00f, 	0.00f, 	-0.10f, -0.20f, 	-0.30f},
	{-0.30f, 	-0.10f, 	0.20f, 	0.30f, 	0.30f, 	0.20f, 	-0.10f, 	-0.30f},
	{-0.30f, 	-0.10f, 	0.30f, 	0.40f, 	0.40f, 	0.30f, 	-0.10f, 	-0.30f},
	{-0.30f, 	-0.10f, 	0.30f, 	0.40f, 	0.40f, 	0.30f, 	-0.10f, 	-0.30f},
	{-0.30f, 	-0.10f, 	0.20f, 	0.30f, 	0.30f, 	0.20f, 	-0.10f, 	-0.30f},
	{-0.30f, 	-0.30f, 	0.00f, 	0.00f, 	0.00f, 	0.00f, 	-0.30f, 	-0.30f},
	{-0.50f, 	-0.30f, 	-0.30f, -0.30f, -0.30f, -0.30f, -0.30f, 	-0.50f}
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

static float fast_mg[2][6][64], fast_eg[2][6][64];//[colore][pezzo][casa]: valore pezzo + tabella, gia' col segno (bianco +, nero -)
static void fast_init(void){
	const float POS_W = 1.0f;//peso della posizione (prova 0.8)
	for(int c=0;c<2;c++) for(int p=PAWN;p<=KING;p++) for(int sq=0;sq<64;sq++){
		Vector2 pos = sq_to_coord(sq);
		if(c==0) pos.y = 7 - pos.y;//nero: specchio verticale
		float base = (p==KING) ? 0.0f : piece_value[p];//i due re si annullano: meglio non sommare 1500 in float
		float sign = c ? 1.0f : -1.0f;
		fast_mg[c][p][sq] = sign*(base + POS_W*pos_mtx[p][MID_GAME][(int)pos.y][(int)pos.x]);
		fast_eg[c][p][sq] = sign*(base + POS_W*pos_mtx[p][END_GAME][(int)pos.y][(int)pos.x]);
	}
}

void mtx_init(void){
	for(uint8_t piece = PAWN; piece<=KING; piece++){

		Mtx8x8* mtx_mg = &pos_mtx[piece][MID_GAME];
		Mtx8x8* mtx_eg = &pos_mtx[piece][END_GAME];

		switch (piece){
			case PAWN:
				*mtx_mg = pawn_mtx_mg;
				*mtx_eg = pawn_mtx_eg;
				break;
			case KNIGHT:
				*mtx_mg = knight;
				*mtx_eg = knight;
				break;
			case BISHOP:
				*mtx_mg = bishop;
				*mtx_eg = bishop;
				break;
			case ROOK:
				*mtx_mg = rook_mg;
				*mtx_eg = rook_eg;
				break;
			case QUEEN:
				*mtx_mg = queen_mg;
				*mtx_eg = queen_eg;
				break;
			case KING:
				*mtx_mg = king_mg;
				*mtx_eg = king_eg;
		}
		// pos_mtx[0][piece][MID_GAME] = invert_mtx(pos_mtx[1][piece][MID_GAME]);
		// pos_mtx[0][piece][END_GAME] = invert_mtx(pos_mtx[1][piece][END_GAME]);
	}
	fast_init();
}

int max_npieces = 32;

// float pos_value(int npieces, float mid_game, float end_game){//media pesata

// 	float start_weight = (float)npieces;
// 	float end_weight = ((float)max_npieces - (float)npieces);
// 	float tot_weight = (float)max_npieces;//magari faccio in base a record.size
	
// 	return ((mid_game)*start_weight + (end_game)*end_weight)/tot_weight;
// }

float moves_value = 0.05f;

float evaluate_pos(Position& state){
	float mg = 0.0f, eg = 0.0f;
	for(int c=0;c<2;c++){
		for(int piece=PAWN; piece<=KING; piece++){
			uint64_t board = state.pieces[c][piece];
			while(board){
				int sq = pop_lsb(board);
				mg += fast_mg[c][piece][sq];
				eg += fast_eg[c][piece][sq];
			}
		}
	}
	int npieces = __builtin_popcountll(state.all);
	float ris = (mg*(float)npieces + eg*(float)(max_npieces - npieces)) / (float)max_npieces;//media pesata una volta sola

	//sviluppo: ogni cavallo/alfiere ancora a casa costa 0.10, sfuma verso il finale
	const uint64_t HOME_W = (1ULL<<1)|(1ULL<<2)|(1ULL<<5)|(1ULL<<6);//b1 c1 f1 g1
	const uint64_t HOME_B = (1ULL<<57)|(1ULL<<58)|(1ULL<<61)|(1ULL<<62);//b8 c8 f8 g8
	int home_w = __builtin_popcountll((state.pieces[1][KNIGHT] | state.pieces[1][BISHOP]) & HOME_W);
	int home_b = __builtin_popcountll((state.pieces[0][KNIGHT] | state.pieces[0][BISHOP]) & HOME_B);
	ris -= 0.10f * (home_w - home_b) * (float)npieces / (float)max_npieces;

	return ris;
}

float move_pst_gain(const Move& m, bool white){//quanto la mossa migliora la tabella posizionale (mid game), per chi muove
	int c = white ? 1 : 0;
	float d = fast_mg[c][m.piece][m.to] - fast_mg[c][m.piece][m.from];
	return white ? d : -d;
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