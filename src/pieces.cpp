#include "pieces.h"
#include "board.h"
#include "game_main.h"
#include "menu_main.h"
#include "board_features.h"
Texture2D texall[2][6];

int piece_value[7] = {1,3,3,5,9,1000,0};
float scalepiece = 0.5f;

void load_pieces_textures(void){
    texall[0][0] = LoadTexture("../images/bp.png");
	texall[0][1] = LoadTexture("../images/bn.png");
	texall[0][2] = LoadTexture("../images/bb.png");
	texall[0][3] = LoadTexture("../images/br.png");
	texall[0][4] = LoadTexture("../images/bq.png");
	texall[0][5] = LoadTexture("../images/bk.png");
	
	texall[1][0] = LoadTexture("../images/wp.png");
	texall[1][1] = LoadTexture("../images/wn.png");
	texall[1][2] = LoadTexture("../images/wb.png");
	texall[1][3] = LoadTexture("../images/wr.png");
	texall[1][4] = LoadTexture("../images/wq.png");
	texall[1][5] = LoadTexture("../images/wk.png");
}

void draw_piece(bool turn, u_int8_t piece, Vector2 pos){

    DrawTextureEx(
        texall[turn][piece],
        pos,
        0,
        scalepiece,
        WHITE
    );
}

void pieces_draw(bool turn){
	Position lastp = currentp;
	for (int i=0;i<6;i++){//type of piece currently looking at
		while(lastp.pieces[turn][i]){
			int sq = pop_lsb(lastp.pieces[turn][i]);
			//draw
			Vector2 coords = sq_to_coord(sq);

			if(settings[TURN]){//se è bianco devo invertire
				coords = invert_coord(coords);
			}
			int col = coords.x;
			int row = coords.y;


			Vector2 pos;
			pos.x = corner.x + squaresize*col + (float)squaresize/2.0f - texall[turn][i].width*scalepiece/2.0f;
			pos.y = corner.y + squaresize*row + (float)squaresize/2.0f - texall[turn][i].height*scalepiece/2.0f;
			draw_piece(turn,i,pos);
		}
	}
}
