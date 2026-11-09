#include "evaluation.h"
#include "board.h"
#include "pieces.h"
#include "board_features.h"
int max_depth = 6;

float eval_val = 0.0f;
float target_eval_val = 0.0f;
float eval_speed = 1.0f;

float evaluate_pos(Position state){
	
    float ris = 0.0f;
	int sq;//per ora è solo valori: poi anche valori in base alla posizione
	//MAPPE DI CONVENIENZA -> devo capire come farle (? da file forse)
	float val;
    for(uint8_t piece = PAWN; piece<=KING; piece++){
        while(state.pieces[0][piece]){
            val =piece_value[piece];
            sq = pop_lsb(state.pieces[0][piece]);

			ris -= val;
        }
        while(state.pieces[1][piece]){
            val=piece_value[piece];
            sq = pop_lsb(state.pieces[1][piece]);

			ris+=val;
        }
    }
    if(held_piece != EMPTY){
        val = (state.w_to_move)? piece_value[held_piece]: -piece_value[held_piece];
		ris+=val;
    }

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