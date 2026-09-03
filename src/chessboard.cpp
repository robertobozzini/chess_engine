
#include "main.h"
#include "lobby.h"
#include "chesspieces.h"
#include "chessboard.h"
#include "features.h"
#include "evaluation.h"

using namespace std;
//la scacchiera la faccio 1000x1000 (4/5 della dimensione)

vector<vector<piece>>pieces;

piece basepiece;

int squaresize, chessboardsize;
Vector2 corner;


Texture2D bp;
Texture2D bn;
Texture2D bb;
Texture2D br;
Texture2D bq;
Texture2D bk;

Texture2D wp;
Texture2D wn;
Texture2D wb;
Texture2D wr;
Texture2D wq;
Texture2D wk;

Texture2D logoing;
float logoingscale = 0.3f;

Vector2 kingposw;
Vector2 kingposb;

bool choosing;
Vector2 choosingpos;
Vector2 lastpos;

void return_to_lobby(Status *status){
	
	event_handler = lobby_handler;
	screen_drawer = lobby_drawer;
	stage = 0;
}

void chessboard_reset(Status *status){
	status->turn = 1;
	
	pieces.clear();
	pieces.resize(8, vector<piece>(8));
	
	for(int row=0; row<8; row++){//creo la scacchiera vuota
		for(int col=0; col<8; col++){
			piece tmp;
			
			//tmp.col=0;
			tmp.pos = (Vector2){(float)col, (float)row};
			
			
			if(row==1){
				tmp.col=0;
				tmp.type=1;
				tmp.img=bp;
			}
			else if(row==6){
				tmp.type=1;
				tmp.col=1;
				tmp.img=wp;
			}
			//cout<<col<<" "<<row<<" "<<tmp.type<<" "<<tmp.col<<endl;
			pieces[col][row]=tmp; // x y
		}
		
	}
	int row=0;
	//pezzi neri
	
	//cavalli
	pieces[1][row].col=0;
	pieces[6][row].col=0;
	pieces[1][row].type=2;
	pieces[6][row].type=2;
	pieces[1][row].img=bn;
	pieces[6][row].img=bn;
	
	//alfieri
	pieces[2][row].col=0;
	pieces[5][row].col=0;
	pieces[2][row].type=3;
	pieces[5][row].type=3;
	pieces[2][row].img=bb;
	pieces[5][row].img=bb;
	
	//torri
	pieces[0][row].col=0;
	pieces[7][row].col=0;
	pieces[0][row].type=4;
	pieces[7][row].type=4;
	pieces[0][row].img=br;
	pieces[7][row].img=br;
	
	//regina e re
	pieces[3][row].col=0;
	pieces[4][row].col=0;
	pieces[3][row].type=5;
	pieces[4][row].type=6;
	pieces[3][row].img=bq;
	pieces[4][row].img=bk;
	
	kingposb = Vector2 {4, (float)row};
	
	row=7;
	//pezzi bianchi
	
	//cavalli
	pieces[1][row].col=1;
	pieces[6][row].col=1;
	pieces[1][row].type=2;
	pieces[6][row].type=2;
	pieces[1][row].img=wn;
	pieces[6][row].img=wn;
	
	//alfieri
	pieces[2][row].col=1;
	pieces[5][row].col=1;
	pieces[2][row].type=3;
	pieces[5][row].type=3;
	pieces[2][row].img=wb;
	pieces[5][row].img=wb;
	
	//torri
	pieces[0][row].col=1;
	pieces[7][row].col=1;
	pieces[0][row].type=4;
	pieces[7][row].type=4;
	pieces[0][row].img=wr;
	pieces[7][row].img=wr;
	
	//regina e re
	pieces[3][row].col=1;
	pieces[4][row].col=1;
	pieces[3][row].type=5;
	pieces[4][row].type=6;
	pieces[3][row].img=wq;
	pieces[4][row].img=wk;
	
	kingposw = Vector2 {4, (float)row};
	
	choosing = 0;
	lastpos = Vector2 {0,0};
	
	reset_arrows(status);
	
	moves.clear();
}

void chessboard_init(Status *status){
	//Carica texture e metti tizi nei loro posti
	
	corner.y=(GetScreenHeight())/9;
	squaresize=corner.y;
	corner.y/=2;
	chessboardsize=squaresize*8;
	corner.x = (GetScreenWidth() - chessboardsize ) - corner.y;
	
	bp = LoadTexture("../images/bp.png");
	bn = LoadTexture("../images/bn.png");
	bb = LoadTexture("../images/bb.png");
	br = LoadTexture("../images/br.png");
	bq = LoadTexture("../images/bq.png");
	bk = LoadTexture("../images/bk.png");
	
	wp = LoadTexture("../images/wp.png");
	wn = LoadTexture("../images/wn.png");
	wb = LoadTexture("../images/wb.png");
	wr = LoadTexture("../images/wr.png");
	wq = LoadTexture("../images/wq.png");
	wk = LoadTexture("../images/wk.png");
	
	logoing = LoadTexture("../images/LOGO.png");
	
	//chessboard_reset(status);

}



bool checklegalmove(Status *status, Vector2 from, Vector2 to){
	//return 1;
	
//	piece eating = pieces[from.x][from.y];
//	piece eaten = pieces[to.x][to.y];
	
	for(Vector2 current:pieces[from.x][from.y].possible_moves){
		if(to == current)return 1;
	}
	return 0;
}

bool check_endgame(bool color){
	
	int n;
	for(int col = 0; col<8; col++){
		for(int row = 0; row<8; row++){
			if(pieces[col][row].col != color)continue;
			pieces[col][row].calculate_moves();
			n = pieces[col][row].possible_moves.size();
			if(n != 0) return 0;//0 = no fine partita
		}
	}
	return 1;
}

void changeturn(Status *status){
	
	status->turn = !status->turn;
	//controlla se è scacco matto o stalemate
	
	
	if(check_endgame(status->turn)){
		//cout<<"finita"<<endl;
		
		bool incheck = check_king_checks(status->turn);
		
		if(incheck){
			//checkmate
			//vince il colore che ha appena fatto il turno
			string vincitore = (status->turn)?"nero":"bianco";
			cout<<"vince il "<<vincitore<<endl;
		}
		else{
			//stalemate
			cout<<"stalemate"<<endl;
		}
		
		//cose (? torna alla lobby)
		chessboard_reset(status);
	}
	reset_arrows(status);
	
	target_eval_val = try_move(status->turn);
	//cout<<target_eval_val<<endl;
}

bool isinboard(Vector2 pos){
	if(pos.x < corner.x or pos.y < corner.y or pos.x > corner.x+chessboardsize or pos.y > corner.y+chessboardsize) 
		return 0;
	return 1;
}

Vector2 chessboardpos(Vector2 mousepos){
	Vector2 chesspos = Vector2Subtract(mousepos, corner);//chessboard pos: quanti pixel dal bordo
	
	int row = floor(((float)chesspos.y)/(float)squaresize);//da 0 a 7
	int col = floor(((float)chesspos.x)/(float)squaresize);
	return Vector2{(float)col,(float)row};
}


void makemove(bool turn, Vector2 oldpos, Vector2 newpos){
	
	pieces[oldpos.x][oldpos.y].onboard = 1;
	
	record lastmove;
	lastmove.taking = pieces[oldpos.x][oldpos.y];
	lastmove.taken = pieces[newpos.x][newpos.y];
	lastmove.moved_set = !lastmove.taking.moved;
	
	if(lastmove.taken.type == 0 && lastmove.taking.type == 1 && lastmove.taking.pos.x != lastmove.taken.pos.x){
		lastmove.wasenpas = 1;
	}
	//moves.push_back({pieces[lastpos.x][lastpos.y], pieces[currpos.x][currpos.y]});
	
	pieces[newpos.x][newpos.y] = pieces[oldpos.x][oldpos.y];
	//pieces[currpos.x][currpos.y].onboard = 1;
	pieces[newpos.x][newpos.y].pos = newpos;
	pieces[newpos.x][newpos.y].moved = 1;
	pieces[newpos.x][newpos.y].enpaspos = -1;
	
	piece tmp;
	pieces[oldpos.x][oldpos.y] = tmp;
	pieces[oldpos.x][oldpos.y].pos = oldpos;
	
	if(pieces[newpos.x][newpos.y].type == 6){
		//è il re: mi segno dove va
		if(turn == 1){
			//bianco
			kingposw = newpos;
		}
		else{
			//nero
			kingposb = newpos;
		}
		
		//arrocco
		if((int)oldpos.x - (int)newpos.x == 2){
			//arrocco lungo
			lastmove.wascastle = 1;
			
			lastmove.rookfrom = {newpos.x-2, newpos.y};
			lastmove.rookto = {newpos.x+1, newpos.y};
			
			pieces[newpos.x+1][newpos.y] = pieces[newpos.x-2][newpos.y];
			pieces[newpos.x+1][newpos.y].moved = 1;
			pieces[newpos.x+1][newpos.y].pos = Vector2{newpos.x+1,newpos.y};
			
			pieces[newpos.x-2][newpos.y] = piece{};
			pieces[newpos.x-2][newpos.y].pos = Vector2{newpos.x - 2, newpos.y};
		}
		else if((int)oldpos.x - (int)newpos.x == -2){
			//arrocco corto
			//cout<<"arrocco corto"<<endl;
			lastmove.wascastle = 1;
			
			lastmove.rookfrom = {newpos.x+1, newpos.y};
			lastmove.rookto = {newpos.x-1, newpos.y};
			
			pieces[newpos.x-1][newpos.y] = pieces[newpos.x+1][newpos.y];
			pieces[newpos.x-1][newpos.y].moved = 1;
			pieces[newpos.x-1][newpos.y].pos = Vector2{newpos.x-1,newpos.y};
			
			pieces[newpos.x+1][newpos.y] = piece{};
			pieces[newpos.x+1][newpos.y].pos = Vector2{newpos.x + 1, newpos.y};
			
		}
		
	}
	else if(pieces[newpos.x][newpos.y].type == 1){
		if(newpos.y == 0 or newpos.y == 7){
			choosingpos = newpos;
			choosing = 1;
		}
		if(lastmove.wasenpas){
			//devo inculare anche quello a fianco
			pieces[lastmove.taken.pos.x][lastmove.taking.pos.y] = piece{};
			pieces[lastmove.taken.pos.x][lastmove.taking.pos.y].pos = {lastmove.taken.pos.x, lastmove.taking.pos.y};
		}
	}
	
	moves.push_back(lastmove);

}

void chessboard_handler(Status *status){
	
	
	if(IsKeyPressed(KEY_ESCAPE)){
		return_to_lobby(status);
	}
	Vector2 mousepos = GetMousePosition();
	
	handle_arrows(status, mousepos);
	
	if(isinboard(mousepos)){
		
		Vector2 chesspos = chessboardpos(mousepos);
		
		int col = chesspos.x;
		int row = chesspos.y;
		
		if(status->inverted){//se sei sotto quando è invertito sei dal nero non dal bianco
			row = 7 - row;
			//col = (col + 8)%8;
		}
		//Ho controllato: funziona
		Vector2 currpos= Vector2{(float)col,(float)row};
		
		if(choosing){
			
			//cose
			/*

			*/
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
				int mulcostant = (status->inverted != status->turn)?1:-1;
				if(currpos.x == choosingpos.x){
					
					if(currpos.y == choosingpos.y){
						//regina
						pieces[choosingpos.x][choosingpos.y].type = 5;
						
						if(status->turn == 1) pieces[choosingpos.x][choosingpos.y].img = wq;
						else pieces[choosingpos.x][choosingpos.y].img = bq;
					}
					else if(currpos.y == choosingpos.y + 1*mulcostant) //ipotizzando sia da sotto
					{
						//torre
						pieces[choosingpos.x][choosingpos.y].type = 4;
						
						if(status->turn == 1) pieces[choosingpos.x][choosingpos.y].img = wr;
						else pieces[choosingpos.x][choosingpos.y].img = br;
					}
					else if(currpos.y == choosingpos.y + 2*mulcostant) //ipotizzando sia da sotto
					{
						//alfiere
						pieces[choosingpos.x][choosingpos.y].type = 3;
						
						if(status->turn == 1) pieces[choosingpos.x][choosingpos.y].img = wb;
						else pieces[choosingpos.x][choosingpos.y].img = bb;
					}
					else if(currpos.y == choosingpos.y + 3*mulcostant) //ipotizzando sia da sotto
					{
						//Cavallo
						pieces[choosingpos.x][choosingpos.y].type = 2;
						
						if(status->turn == 1) pieces[choosingpos.x][choosingpos.y].img = wn;
						else pieces[choosingpos.x][choosingpos.y].img = bn;
					}
					else{
						return;
					}
					choosing = 0;
					changeturn(status);
				}
			}
			return;
		}
		
		if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){//sicuramente non stavi tenendo nulla se premi
			if(pieces[col][row].type != 0 && pieces[col][row].col == status->turn){
				
				status->isholding = 1;
				pieces[col][row].onboard = 0;
				lastpos = currpos;
				
				pieces[col][row].calculate_moves();
			}
		}
		if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && status->isholding){
			
			bool islegal = checklegalmove(status, lastpos, currpos);
			status->isholding = 0;
			//cout<<islegal<<" "<<pieces[lastpos.x][lastpos.y].possible_moves.size()<<endl;
			
			if(islegal){
				
				makemove(status->turn, lastpos, currpos);
				
				if(!choosing)changeturn(status);
			}
			else{
				pieces[lastpos.x][lastpos.y].onboard = 1;//torna al posto di prima e basta
			}
		}
		
	}
	
	
}

void chessboard_drawer(Status *status){
	ClearBackground(BLACK);
	
	//rifai
	
	
	
	DrawTextureEx(
				  logoing,
				  (Vector2){corner.y - squaresize/4,corner.y - squaresize/4},
				  0,
				  logoingscale,
				  WHITE
				  );
	
	
	DrawRectangle(corner.x, corner.y, chessboardsize, chessboardsize, BEIGE);
	
	if(status->inverted){
		//simmetria
	}
	//inizia da in alto a sinistra: a8
	int row;
	for(int col = 0; col < 8; col++){//uno ogni 2 quadretti
		for(int orow = 0; orow < 8; orow++){
			
			if(status->inverted)
				row = 7 - orow;
			else 
				row = orow;
			
			if(( col + row )%2==1){
				int top=corner.y+row*squaresize;
				int left=corner.x+col*squaresize;
				DrawRectangle(left, top, squaresize, squaresize, DARKBROWN);
			}
			
		}
	}		
	
	if(!moves.empty()){
		Vector2 pos1 = moves[moves.size()-1].taking.pos;
		Vector2 pos2 = moves[moves.size()-1].taken.pos;
		
		int row = pos1.y;
		int col = pos1.x;
		
		if(status->inverted)
			row = 7 - row;
		
		int top=corner.y+row*squaresize;
		int left=corner.x+col*squaresize;
		float transparency = ((row+col)%2 == 1)?0.5:0.3;
		DrawRectangle(left, top, squaresize, squaresize, Fade(YELLOW, transparency));
		
		row = pos2.y;
		col = pos2.x;
		
		if(status->inverted)
			row = 7 - row;
		
		top=corner.y+row*squaresize;
		left=corner.x+col*squaresize;
		transparency = ((row+col)%2 == 1)?0.5:0.3;
		DrawRectangle(left, top, squaresize, squaresize, Fade(YELLOW,transparency));
	}
	
	
	for(int col=0; col<8; col++){
		for(int row=0; row<8; row++){
			pieces[col][row].draw(status);
		}
	}
	
	//draw di dove puo andare
	if(status->isholding){
		for(Vector2 pos: pieces[lastpos.x][lastpos.y].possible_moves){
			Vector2 center;
			row = pos.y;
			
			if(status->inverted)
				row = 7 - row;
			
			center.x = corner.x + squaresize*pos.x + squaresize/2;
			center.y = corner.y + squaresize*row + squaresize/2;
			
			if(pieces[pos.x][pos.y].type == 0){
				DrawCircleV(center, squaresize*0.15f, Fade(BROWN,0.35f));
			}
			else{
				DrawRing(center, squaresize*0.38f, squaresize*0.45f, 0, 360, 36, Fade(BROWN, 0.6f));
			}
		}
	}
	
	if(choosing){
		int mulcostant = (status->inverted != status->turn)?1:-1;
		for(int i = 0; i < 4; i++){
			int col = choosingpos.x;
			int row = choosingpos.y;
			
			if(status->inverted)
				row = 7 - row;

			

			int top=corner.y+(row + i*mulcostant)*squaresize;
			int left=corner.x+col*squaresize;
			
			Color color = (status->turn)?WHITE:BLACK;
			
			DrawRectangle(left, top, squaresize, squaresize, color);
			
			Texture2D img;
			Vector2 posi;
			
			if(i == 0){
				if(status->turn) img = wq;
				else img = bq;
			}
			else if(i==1){
				if(status->turn) img = wr;
				else img = br;
			}
			else if(i==2){
				if(status->turn) img = wb;
				else img = bb;
			}
			else if(i==3){
				if(status->turn) img = wn;
				else img = bn;
			}
			
			posi.x=corner.x + squaresize*(col) + squaresize/2 - img.width*scalepiece/2;
			posi.y=corner.y + squaresize*(row + i * mulcostant) + squaresize/2 - img.height*scalepiece/2;
			
			DrawTextureEx(
				img,
				posi,
				0,
				scalepiece,
				WHITE
			);
		}
		
	}
	drawArrows(status);
	
	eval_bar_draw(status);
}
