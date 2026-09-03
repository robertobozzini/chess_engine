#include "chesspieces.h"
#include "chessboard.h"
#include "evaluation.h"
using namespace std;

float scalepiece = 0.5f;

Vector2 knightdirs[8] = {
	{2,1},{2,-1},{-2,1},{-2,-1},{1,2},{1,-2},{-1,2},{-1,-2}
};

Vector2 kingdirs[8] = {
	{0,1},{0,-1},{1,0},{-1,0},{1,1},{1,-1},{-1,1},{-1,-1}
};

Vector2 straightdirs[4] = { {0,1},{0,-1},{1,0},{-1,0} };

Vector2 diagdirs[4] = { {1,1},{1,-1},{-1,1},{-1,-1} };

void piece::draw(Status *status){
	
	if(type == 0) return;//è vuoto
	//cout<<type<<endl;
	Vector2 posi;
	if(onboard){
		int row = pos.y;
		if(status->inverted)row = 7 - row;
		posi.x=corner.x + squaresize*(pos.x) + squaresize/2 - img.width*scalepiece/2;
		posi.y=corner.y + squaresize*(row) + squaresize/2 - img.height*scalepiece/2;
		//cout<<pos.x<<" "<<pos.y<<endl;
	}
	else{
		//devo capire come fare, una cosa alla volta
		
		posi=GetMousePosition();
		posi.x -= img.width*scalepiece/2;
		posi.y -= img.height*scalepiece/2;
	}
		
		
	DrawTextureEx(
		img,
		posi,
		0,
		scalepiece,
		WHITE
	);
}
bool inboard(Vector2 pos){
	return !(pos.x > 7 or pos.y > 7 or pos.x < 0 or pos.y < 0);
}


bool is_attacked(Vector2 pos, bool attacker_color){
	
	//pedone
	int diry=(attacker_color == 1)?-1:1;
	if(inboard(Vector2Add(Vector2{ 1, (float)diry}, pos)) && pieces[1+pos.x][diry+pos.y].type == 1 && pieces[1+pos.x][diry+pos.y].col == attacker_color) 
		return 1;
	if(inboard(Vector2Add(Vector2{ -1, (float)diry}, pos)) && pieces[-1+pos.x][diry+pos.y].type == 1 && pieces[-1+pos.x][diry+pos.y].col == attacker_color) 
		return 1;
	
	for(Vector2 dir:knightdirs){
		
		Vector2 curp = Vector2Add(pos,dir);
		if(inboard(curp) && pieces[curp.x][curp.y].type == 2 && pieces[curp.x][curp.y].col == attacker_color)
			return 1;
	}
	for(Vector2 dir:diagdirs){
		
		Vector2 curp = Vector2Add(pos,dir);
		
		while(inboard(curp) && pieces[curp.x][curp.y].type == 0) 
			curp = Vector2Add(curp,dir);
		
		if(inboard(curp) && pieces[curp.x][curp.y].col == attacker_color && (pieces[curp.x][curp.y].type == 3 or pieces[curp.x][curp.y].type == 5))
			return 1;
	}
	for(Vector2 dir:straightdirs){
		
		Vector2 curp = Vector2Add(pos,dir);
		
		while(inboard(curp) && pieces[curp.x][curp.y].type == 0) 
			curp = Vector2Add(curp,dir);
		
		if(inboard(curp) && pieces[curp.x][curp.y].col == attacker_color && (pieces[curp.x][curp.y].type == 4 or pieces[curp.x][curp.y].type == 5))
			return 1;
	}
	
	for(Vector2 dir:kingdirs){
		
		Vector2 curp = Vector2Add(pos,dir);
		if(inboard(curp) && pieces[curp.x][curp.y].type == 6 && pieces[curp.x][curp.y].col == attacker_color)
			return 1;
	}
	
	return 0;
}

bool check_king_checks(bool col){//colore di che re vuoi controllare se puo venire preso
	
	Vector2 kingpos;
	
	if(col == 0)
		kingpos = kingposb;
	else 
		kingpos = kingposw;
	
	return is_attacked(kingpos, !col);
}

bool would_be_check(bool col, Vector2 pos, Vector2 newpos){//colore di che re vuoi controllare se puo venire preso
	
	Vector2 kingpos;
	
	if(col == 0)
		kingpos = kingposb;
	else 
		kingpos = kingposw;
	
	piece savedfrom = pieces[pos.x][pos.y];
	piece savedto = pieces[newpos.x][newpos.y];
	//return is_attacked(kingpos, !col);
	
	//devo fare en passant pure qua
	pieces[newpos.x][newpos.y] = pieces[pos.x][pos.y];
	pieces[newpos.x][newpos.y].pos = newpos;
	pieces[pos.x][pos.y] = piece{};
	pieces[pos.x][pos.y].pos = pos;
	
	if(savedfrom.type == 6)kingpos = newpos;
	
	bool incheck = is_attacked(kingpos, !col);
	
	pieces[pos.x][pos.y] = savedfrom;
	pieces[newpos.x][newpos.y] = savedto;
	
	return incheck;
}

void lineadd(vector<Vector2>&list, Vector2 pos, Vector2 dir, bool col){
	
	Vector2 oldpos = pos;
	pos = Vector2Add(pos, dir);// non posso muovermi nella mia stessa posizione
	
	if(!inboard(pos))return;
	
	while(pieces[pos.x][pos.y].type==0){//mentre è libero
		if(!would_be_check(col, oldpos, pos))
			list.push_back(pos);
		pos = Vector2Add(pos, dir);
		
		if(!inboard(pos))return;
	}
	//ho trovato qualcosa
	if(pieces[pos.x][pos.y].col !=col && !would_be_check(col, oldpos, pos))
		list.push_back(pos);//se è di un colore diverso allora lo posso catturare
}

void staticmove(vector<Vector2>&list, Vector2 pos, Vector2 dir, bool col){//andare in un preciso punto e prendere il pezzo
	Vector2 oldpos = pos;
	pos = Vector2Add(pos, dir);
	if(!inboard(pos))return;
	
	if(pieces[pos.x][pos.y].type == 0 or pieces[pos.x][pos.y].col != col){
		if(!would_be_check(col, oldpos, pos))
			list.push_back(pos);
	}
}

void piece::calculate_moves(){
	// a scacchi e pin ci pensiamo dopo
	possible_moves.clear();
	
	if(type == 0)return;
	
	Vector2 dir;
	if(type == 1){
		enpaspos = -1;
		dir = (col==1)?Vector2{0,-1}:Vector2{0,1};// in cui si muove
		//bool free = (pieces[pos.x][pos.y+dir.y].type ==0 );
		if(pieces[pos.x][pos.y+dir.y].type == 0 ){
			
			staticmove(possible_moves, pos, dir, col);
			
			if(!moved && pieces[pos.x][pos.y + dir.y*2].type == 0){
				//dir.y = dir.y*2;
				staticmove(possible_moves, pos, Vector2{0, dir.y*2}, col);
			}
		}
		if(pos.x+1 <= 7 && pieces[pos.x+1][pos.y+dir.y].type != 0 && pieces[pos.x+1][pos.y+dir.y].col != col){
			dir.x = 1;
			staticmove(possible_moves, pos, dir, col);
		}
		else if(pos.x+1 <= 7 && pieces[pos.x+1][pos.y+dir.y].type == 0 && pieces[pos.x+1][pos.y].type == 1 && !moves.empty()){
			record lastmove = moves[moves.size()-1];
			if(lastmove.taken.pos == Vector2{pos.x+1, pos.y} && abs(lastmove.taking.pos.y - lastmove.taken.pos.y) == 2){//cosi sei sicuro che è: pedone, colore opposto, che ha fatto 2 avanti
				//fai cose
				bool onb = onboard;
				vector<Vector2>tmp_moves = possible_moves;
				
				makemove(col, pos, {pos.x+1, pos.y +dir.y});
				
				bool va =!check_king_checks(col);
				
				undo_move();
				
				possible_moves = tmp_moves;
				if(va){
					//cout<<"qua va?"<<endl;
					enpaspos = possible_moves.size();
					possible_moves.push_back({pos.x+1, pos.y +dir.y});
				}
				onboard = onb;
			}
			
		}
		if(pos.x-1 >= 0 && pieces[pos.x-1][pos.y+dir.y].type != 0 && pieces[pos.x-1][pos.y+dir.y].col != col){
			dir.x = -1;
			staticmove(possible_moves, pos, dir, col);
		}
		else if(pos.x-1 >= 0 && pieces[pos.x-1][pos.y+dir.y].type == 0 && pieces[pos.x-1][pos.y].type == 1 && !moves.empty()){
			record lastmove = moves[moves.size()-1];
			if(lastmove.taken.pos == Vector2{pos.x-1, pos.y} && abs(lastmove.taking.pos.y - lastmove.taken.pos.y) == 2){//cosi sei sicuro che è: pedone, colore opposto, che ha fatto 2 avanti
				//fai cose
					bool onb = onboard;
					vector<Vector2>tmp_moves = possible_moves;
					
					makemove(col, pos, {pos.x-1, pos.y +dir.y});
					
					bool va =!check_king_checks(col);
					
					undo_move();
					
					possible_moves = tmp_moves;
					if(va){
						//cout<<"qua va?"<<endl;
						enpaspos = possible_moves.size();
						possible_moves.push_back({pos.x-1, pos.y +dir.y});
					}
					onboard = onb;
			}
		}
		//enpassant
		
		
	}
	if(type == 3 or type == 5){
		
		//alfiere o regina:
		for(Vector2 dir:diagdirs){
			lineadd(possible_moves, pos, dir, col);
		}
		
	}
	if(type == 4 or type == 5){
		
		//torre o regina
		for(Vector2 dir:straightdirs){
			lineadd(possible_moves, pos, dir, col);
		}
	}
	
	if(type == 2){
		//cavallo
		
		//ogni combinazione di +-2 e +-1 o +-1 e +-2
		for(Vector2 dir:knightdirs){
			staticmove(possible_moves, pos, dir, col);
		}
	}
	if(type == 6){
		//re
		for(Vector2 dir:kingdirs){
			staticmove(possible_moves, pos, dir, col);
		}
		
		if(!moved && !check_king_checks(col)){
			if(pieces[pos.x + 1][pos.y].type == 0 && !is_attacked(Vector2{pos.x+1,pos.y}, !col) && 
			   pieces[pos.x + 2][pos.y].type == 0 && !is_attacked(Vector2{pos.x+2,pos.y}, !col) &&
			   !pieces[pos.x + 3][pos.y].moved)
			{//arrocco corto
				Vector2 move =  Vector2{pos.x+2,pos.y};
				if(!would_be_check(col, pos, move))
					possible_moves.push_back(move);
			}
			//cout<<pos.x<<" "<<pos.y<<endl;
			
			if(pieces[pos.x - 1][pos.y].type == 0 && !is_attacked(Vector2{pos.x-1,pos.y}, !col) && 
			   pieces[pos.x - 2][pos.y].type == 0 && !is_attacked(Vector2{pos.x-2,pos.y}, !col) &&
			   pieces[pos.x - 3][pos.y].type == 0 && !pieces[pos.x - 4][pos.y].moved)
			{ //arrocco lungo
				Vector2 move =  Vector2{pos.x-2,pos.y};
				if(!would_be_check(col, pos, move))
					possible_moves.push_back(move);
			}
			
		}
		
	}
}
