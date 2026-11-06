#include "board_features.h"
#include "board.h"
#include "pieces.h"
#include "menu_main.h"

using namespace std;



u_int8_t held_piece;// = EMPTY;//empty
int held_piece_sq;// = NO_SQ;
Vector2 st_arrow_pos = (Vector2){-1,-1};
vector<pair<Vector2,Vector2>>arrows;
float scale_arrow_width = 0.15f;
void handle_mouse(void){
    Vector2 mouse_position = GetMousePosition();
    if(!on_board(mouse_position)){
		if(held_piece!=EMPTY){
			set_board_piece(currentp, currentp.w_to_move, held_piece_sq, held_piece);
			held_piece=EMPTY;
			held_piece_sq = NO_SQ;
		}
		return;
	}

    Vector2 board_pos = chessboard_pos(mouse_position);

	if(!settings[TURN])//giochi come il nero:devi invertire
		board_pos=invert_coord(board_pos);
	
    int sq = coord_to_sq(board_pos);
	//cout<<"ti trovi in "<<board_pos.x<<" "<<board_pos.y<<" al quadrato n "<<sq<<endl;
    //handle del pezzo in mano
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && is_occ(currentp.occ[currentp.w_to_move], sq)){//sicuramente non avevo pezzi in mano
        held_piece = currentp.type_on[sq];
		held_piece_sq = sq;

		//tolgo pezzo dalla board
		clear_board_piece(currentp, currentp.w_to_move, sq, held_piece);
    }
    else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && held_piece != EMPTY){

		bool legal_move = true;
		
		if(legal_move){//moves
			set_board_piece(currentp, currentp.w_to_move, sq, held_piece);
			change_turn();
		}
		else{//return to original spot
			set_board_piece(currentp, currentp.w_to_move, held_piece_sq, held_piece);
		}
        held_piece=EMPTY;
		held_piece_sq = NO_SQ;
    }
    
    //arrows
    if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
        st_arrow_pos = board_pos;

    }
    else if(IsMouseButtonReleased(MOUSE_BUTTON_RIGHT) && st_arrow_pos.x !=-1){
        if(st_arrow_pos != board_pos)
            arrows.push_back({st_arrow_pos,board_pos});
        
        st_arrow_pos=(Vector2){-1,-1};
    }

}


void drawArrow(Vector2 start, Vector2 end){
	
	start = { corner.x + squaresize*start.x + squaresize/2, corner.y + squaresize*start.y + squaresize/2 };
	
	end = { corner.x + squaresize*end.x + squaresize/2, corner.y + squaresize*end.y + squaresize/2 };
	
	Color color = Fade(ORANGE, 0.5f);
	
	Vector2 dir = Vector2Subtract(end, start);
	float len = Vector2Length(dir);
	dir = Vector2Scale(dir, 1.0f/len);
	
	Vector2 perp = Vector2{-dir.y, dir.x};
	
	float HeadLen = scale_arrow_width * squaresize * 3.5f;
	float HeadWid = scale_arrow_width * squaresize * 3.0f;
	
	Vector2 shaftEnd = Vector2Subtract(end, Vector2Scale(dir, HeadLen));
	
	DrawLineEx(start, shaftEnd, scale_arrow_width * squaresize, color);
	
	Vector2 left  = Vector2Add(shaftEnd, Vector2Scale(perp, HeadWid/2));
	Vector2 right = Vector2Subtract(shaftEnd, Vector2Scale(perp, HeadWid/2));
	
	DrawTriangle(end, right, left, color);
	
}

void draw_features(void){
	//held piece

	if(held_piece != EMPTY){
		Vector2 pos = GetMousePosition();
		pos.x-=texall[currentp.w_to_move][held_piece].width/2.0f*scalepiece;
		pos.y-=texall[currentp.w_to_move][held_piece].height/2.0f*scalepiece;
		draw_piece(currentp.w_to_move,held_piece, pos);
	}

	//arrows
	for(auto [start,end]:arrows){
		drawArrow(start,end);
	}
}