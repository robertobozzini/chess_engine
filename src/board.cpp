#include "board.h"
#include "game_main.h"
#include "menu_main.h"
using namespace std;

Vector2 corner;
int squaresize, chessboardsize;

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



void board_reset(void){
    return;
}

void board_init(void){
    corner.y=(ScreenHeight)/9;
	squaresize=corner.y;
	corner.y/=2;
	chessboardsize=squaresize*8;
	corner.x = (ScreenWidth - chessboardsize ) - corner.y;

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
	
	board_reset();
}

void board_draw(void){

    DrawRectangle(corner.x, corner.y, chessboardsize, chessboardsize, BEIGE);

    int row;
	for(int col = 0; col < 8; col++){//uno ogni 2 quadretti
		for(int orow = 0; orow < 8; orow++){
			
			if(!settings[1])//giochi come il nero:devi invertire
				row = 7 - orow;
			else 
				row = orow;
			cout<<settings[1]<<" "<<col<<" "<<row<<endl;
			if(( col + row )%2==1){
				int top=corner.y+row*squaresize;
				int left=corner.x+col*squaresize;
				DrawRectangle(left, top, squaresize, squaresize, DARKBROWN);
			}
			
		}
	}		
}