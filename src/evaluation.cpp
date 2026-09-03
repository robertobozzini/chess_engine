#include "chessboard.h"
#include "lobby.h"
#include "evaluation.h"
#include "chesspieces.h"
#include "main.h"

using namespace std;





vector<record>moves;//pezzo first si è mosso in pos di pezzo second

int max_depth = 3;

float eval_val = 0.0f;
float target_eval_val = 0.0f;

void undo_move(void){
	
	if(moves.empty()) return;
	
	record lastmove = moves[moves.size()-1];
	
	piece taken = lastmove.taken;
	piece taking = lastmove.taking;
	
	if(lastmove.moved_set)taking.moved = 0;
	
	pieces[taken.pos.x][taken.pos.y] = taken;
	pieces[taking.pos.x][taking.pos.y] = taking;
	
	
	
	piece tmp;
	
	if(taking.type == 6){//il re non puo essere catturato
		if(taking.col == 1){
			//turno del bianco 
			kingposw = taking.pos;
		}
		else{
			kingposb = taking.pos;
		}
		if(lastmove.wascastle){
			//resetta l'arrocco'
			pieces[lastmove.rookfrom.x][lastmove.rookfrom.y] = pieces[lastmove.rookto.x][lastmove.rookto.y];
			pieces[lastmove.rookfrom.x][lastmove.rookfrom.y].pos = lastmove.rookfrom;
			pieces[lastmove.rookfrom.x][lastmove.rookfrom.y].moved = 0;
			
			
			pieces[lastmove.rookto.x][lastmove.rookto.y] = tmp;
			pieces[lastmove.rookto.x][lastmove.rookto.y].pos = lastmove.rookto;
		}
		
	}
	else if(taking.type == 1 && lastmove.wasenpas){
		//resetta l'en passant
		pieces[taken.pos.x][taking.pos.y] = tmp;
		pieces[taken.pos.x][taking.pos.y].type = 1;
		pieces[taken.pos.x][taking.pos.y].pos = {taken.pos.x, taking.pos.y};
		pieces[taken.pos.x][taking.pos.y].col = !taking.col;
		pieces[taken.pos.x][taking.pos.y].img = (pieces[taken.pos.x][taking.pos.y].col)?wp:bp;
		pieces[taken.pos.x][taking.pos.y].moved = 1;
	}
	
	moves.pop_back();
}

float evaluate_pieces(bool turn){
	//da il valore dei pezzi: situazione di materiale
	float val = 0.0f;
	for(int col = 0; col < 8; col++){
		for(int row = 0; row < 8; row++){
			if(pieces[col][row].type == 0 or pieces[col][row].type == 6)continue;
			int add = (pieces[col][row].col == 1)?1:-1;
			
			if(pieces[col][row].type == 1)add *=1;
			else if(pieces[col][row].type == 2 or pieces[col][row].type == 3)add *=3;
			else if(pieces[col][row].type == 4)add *=5;
			else if(pieces[col][row].type == 5)add *=9;
			
			val+=add;
		}
	}
//	if(check_endgame(turn)){
//		if(check_king_checks(turn)){
//			val = (turn)?-105.0f:105.0f;//il massimo è 103, se faccio abs(val)>104 allora è mate 
//		}
//		else val = 0.0f;
//	}
	return val;
}

pair<piece, piece>bestmove;

float try_move(bool turn, int depth, float alphap, float betap){
	//provi ogni mossa, poi ti segni la mossa (con evaluate pieces) migliore e gli torni indietro quella
	//turn è il turno che fai ora
	if(depth == max_depth)return evaluate_pieces(turn);
	int mulcostant = (turn)?1:-1;
	//RICONTROLLARE (- * - 105 = 105)
	float bestval = (turn)?-105.0f:105.0f;//punteggio massimo è 104, 105 significa scacco matto
	
	float  val;
	pair<piece, piece>move;
	
	
	bool cutoff = 0;
	
	bool mossa = 0;
	for(int col = 0; col < 8 && !cutoff; col++){
		for(int row = 0; row < 8 && !cutoff; row++){
			
			//if(cutoff)break;
			
			if(pieces[col][row].type == 0 or pieces[col][row].col != turn)continue;
			pieces[col][row].calculate_moves();
			
			if(!pieces[col][row].possible_moves.empty())mossa = 1;
				
			for(int i = 0; i < pieces[col][row].possible_moves.size(); i++){
				//muovo, calcolo, faccio undo
				
				Vector2 pos = pieces[col][row].possible_moves[i];
				
				makemove(turn, {(float)col, (float)row}, pos);
				
				if(pieces[pos.x][pos.y].type == 1 && choosing){
						
					choosing = 0;
					
					pieces[pos.x][pos.y].type = 2;
					pieces[pos.x][pos.y].img = (turn)?wn:bn;
					
					val = try_move(!turn, depth+1, alphap, betap);
					
					
					if(val*mulcostant > bestval*mulcostant){//se sei nero vuoi che il val sia minore, se sei bianco vuoi che sia maggiore
						bestval = val;
						
						if(turn){
							if(bestval > alphap) alphap = bestval;
						}
						else{
							if(bestval < betap) betap = bestval;
						}
					}
					
					if(alphap >= betap){
						undo_move();
						cutoff = 1;
						break;
					}
					
					pieces[pos.x][pos.y].type = 3;
					pieces[pos.x][pos.y].img = (turn)?wb:bb;
					
					val = try_move(!turn, depth+1, alphap, betap);
					
					if(val*mulcostant > bestval*mulcostant){//se sei nero vuoi che il val sia minore, se sei bianco vuoi che sia maggiore
						bestval = val;
						if(turn){
							if(bestval > alphap) alphap = bestval;
						}
						else{
							if(bestval < betap) betap = bestval;
						}
					}
					
					if(alphap >= betap){
						undo_move();
						cutoff = 1;
						break;
					}
					
					pieces[pos.x][pos.y].type = 4;
					pieces[pos.x][pos.y].img = (turn)?wr:br;
					
					val = try_move(!turn, depth+1, alphap, betap);
					
					if(val*mulcostant > bestval*mulcostant){//se sei nero vuoi che il val sia minore, se sei bianco vuoi che sia maggiore
						bestval = val;
						if(turn){
							if(bestval > alphap) alphap = bestval;
						}
						else{
							if(bestval < betap) betap = bestval;
						}
					}
					
					if(alphap >= betap){
						undo_move();
						cutoff = 1;
						break;
					}
					
					pieces[pos.x][pos.y].type = 5;
					pieces[pos.x][pos.y].img = (turn)?wq:bq;
					
					val = try_move(!turn, depth+1, alphap, betap);
					
					if(val*mulcostant > bestval*mulcostant){//se sei nero vuoi che il val sia minore, se sei bianco vuoi che sia maggiore
						bestval = val;
						if(turn){
							if(bestval > alphap) alphap = bestval;
						}
						else{
							if(bestval < betap) betap = bestval;
						}
					}
					undo_move();
					
					if(alphap >= betap){
						cutoff = 1;
						break;
					}
					else continue;
				
				}
				
				val = try_move(!turn, depth+1, alphap, betap);
				//else val = evaluate_pieces(turn);
				
				if(val*mulcostant > bestval*mulcostant){//se sei nero vuoi che il val sia minore, se sei bianco vuoi che sia maggiore
					bestval = val;
					if(turn){
						if(bestval > alphap) alphap = bestval;
					}
					else{
						if(bestval < betap) betap = bestval;
					}
				}
				undo_move();
				if(alphap >= betap){
					cutoff = 1;
					break;
				}
			}
			
		}
		
	}
	if(!mossa){
		//non puoi fare mosse:
		if(check_king_checks(turn)){
			val = (turn)?-105.0f:105.0f;//il massimo è 103, se faccio abs(val)>104 allora è mate 
		}
		else val = 0.0f;
		return val;
	}//return evaluate_pieces(turn);
	return bestval;
	
	//int val = evaluate_pieces(turn);
}




void eval_bar_draw(Status *status){
	
	float k = 8;
	float eval_speed = 1.0f;
	
	float barheight = chessboardsize;
	float barwidth = squaresize/2;
	//lo spostamento dovrebbe essere a livello logaritmico es log(|val|) o boh, comunque su un totale di 100 - 100
	eval_val += (target_eval_val - eval_val) * eval_speed * GetFrameTime();//si avvicina a target_eval_val
	
	if(fabs(target_eval_val - eval_val) < 0.05f)
		eval_val = target_eval_val;
	
	float barprop;// tra -1 e +1, indica quanto verso un lato va la riga
	if(fabs(eval_val)>104.0f)
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
	
	//ora manca grafica
}
