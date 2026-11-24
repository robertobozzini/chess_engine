#include "board_features.h"
#include "board.h"
#include "pieces.h"
#include "menu_main.h"
#include "board_features.h"
#include "evaluation.h"
#include "buttons.h"
#include "buttons_specifics.h"
#include "search.h"


using namespace std;

uint8_t held_piece;// = EMPTY;//empty
int held_piece_sq;// = NO_SQ;
Vector2 st_arrow_pos = (Vector2){-1,-1};
set<pair<Vector2,Vector2>>arrows;
float scale_arrow_width = 0.15f;

uint64_t red_squares;

Button take_back_button;
Button restart_button;
Button lobby_button;
Font board_font;
Font text_font;

void take_back_move(void){
	//gestisco anche legality
	take_back_button.state = 0;
	int rep = (settings[BOT])?2:1;
	for(int i=0;i<rep;i++){
			
		if(record.size() == 1){
			//starting position
			search_gen = 0;
			eval_val = 0.0f;
			target_eval_val = 0.0f;
			return;
		}
		record.pop_back();
		currentp = record[record.size()-1];
		current_legality = legality(currentp);
	}



	if((currentp.w_to_move == !settings[TURN]) && settings[BOT]){
		bot_asyinc(currentp, current_legality);

	}
	else if(settings[EVAL]){
		eval_asyinc(currentp);
	}
}

void rematch(void){
	
	turn_button();
	menu_buttons[TURN].state = !menu_buttons[TURN].state;

	board_reset();
}

void game_buttons_init(void){

	fontsize = 25;
    board_font = LoadFontEx("../Fonts/Inter/static/Inter_28pt-ExtraBold.ttf", fontsize, NULL, 0);
	text_font = LoadFontEx("../Fonts/Inter/static/Inter_28pt-ExtraBold.ttf", 90, NULL, 0);

	Vector2 button_size;
	button_size.x = ScreenWidth/6.0f;
    button_size.y = ScreenHeight/16.0f;

	Vector2 button_pos;
    button_pos.x = ScreenWidth*1.0f/6.0f - button_size.x/2.0f;
    button_pos.y = ScreenHeight*1.5f/5.0f;

	take_back_button.init(button_pos, button_size, "take back move", fontsize, board_font, take_back_move);
	
	button_pos.y += 2*button_size.y;

	restart_button.init(button_pos, button_size, "rematch", fontsize, board_font, rematch);
	
	button_pos.y += 2*button_size.y;
	//newpos
	
	lobby_button.init(button_pos, button_size, "return to lobby", fontsize, board_font, nullptr);

	

	
}

void handle_turn(int sq){

	
	if((currentp.w_to_move == !settings[TURN]) && settings[BOT])//bots turn
	{
		if(record.size()==1)
			bot_asyinc(currentp, current_legality);
		return;
	}

	if(choosing){
		//gestisco
		int promosq = mv.to;//square di dove si trova: da qua shifto verso alto o basso
		bool turn = currentp.w_to_move;

		int shift_constant = (turn)? -8:8;

		if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
			for(uint8_t p=KNIGHT; p<=QUEEN; p++){
				if(sq == (promosq + (p-1)*shift_constant)){
					//im on that square
					mv.promo = p;
					//currentp.choosing = 0;
					choosing = 0;
					change_turn(mv);
					break;
				}
			}
		} 

		return;
	}

		//cout<<"ti trovi in "<<board_pos.x<<" "<<board_pos.y<<" al quadrato n "<<sq<<endl;
    //handle del pezzo in mano
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && is_occ(currentp.occ[currentp.w_to_move], sq)){//sicuramente non avevo pezzi in mano
        held_piece = currentp.type_on[sq];
		held_piece_sq = sq;

		//tolgo pezzo dalla board
    }
	else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && held_piece != EMPTY){
		if(held_piece_sq == sq){
			red_squares = 0ULL;
			arrows.clear();
		}
		u_int64_t legal_moves = find_moves(currentp, current_legality, held_piece, held_piece_sq);
		bool legal_move = is_occ(legal_moves, sq);
		
		if(legal_move){//moves

			mv = build_move(currentp, held_piece_sq, sq);

			if(mv.piece == PAWN && (sq/8 == 0 || sq/8 == 7))
				choosing = 1;  
			else 
				change_turn(mv);
		}

        held_piece=EMPTY;
		held_piece_sq = NO_SQ;
    }
}

void handle_mouse(void){
    Vector2 mouse_position = GetMousePosition();

	take_back_button.button_event_handler();
	restart_button.button_event_handler();
	lobby_button.button_event_handler();

	if(currentp.end_game != NO_ENDG){

		return;
	}

	if(bot_ready){
		change_turn(bot_move);
		bot_ready = false;
	}

    if(!on_board(mouse_position)){

		if(held_piece!=EMPTY){
			held_piece = EMPTY;
			held_piece_sq = NO_SQ;
		}

		if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
			arrows.clear();
			red_squares = 0ULL;
		}
		return;
	}

    Vector2 board_pos = chessboard_pos(mouse_position);


	if(!settings[TURN])//giochi come il nero:devi invertire
		board_pos=invert_coord(board_pos);
	int sq = coord_to_sq(board_pos);


	//arrows
	if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
		arrows.clear();
		red_squares=0ULL;
	}

    if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
        st_arrow_pos = board_pos;

    }
    else if(IsMouseButtonReleased(MOUSE_BUTTON_RIGHT) && st_arrow_pos.x !=-1){
        if(st_arrow_pos != board_pos){
			//try to find if already in
			if(arrows.find({st_arrow_pos,board_pos}) != arrows.end())
				arrows.erase({st_arrow_pos,board_pos});
			else
				arrows.insert({st_arrow_pos,board_pos});
		}
        else {
			//red square
			if(is_occ(red_squares, sq))
				clear_bit(red_squares, sq);
			else
				set_bit(red_squares, sq);
		}
		
			
        st_arrow_pos=(Vector2){-1,-1};
    }

	
	handle_turn(sq);


    

}

bool handle_features(void){

	handle_mouse();

	return lobby_button.state;
	//return false;
	
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

void draw_attack_table(uint64_t table){
	while(table){
		int sq = pop_lsb(table);
		Vector2 pos = sq_to_coord(sq);

		if(!settings[TURN])
			pos = invert_coord(pos);

		Vector2 center;
		center.x = corner.x + squaresize*pos.x + squaresize/2.0f;
		center.y = corner.y + squaresize*pos.y + squaresize/2.0f;

		if(is_occ(currentp.all, sq)){
			DrawRing(center, squaresize*0.38f, squaresize*0.45f, 0, 360, 36, Fade(BROWN, 0.6f));
		}
		else{
			DrawCircleV(center, squaresize*0.15f, Fade(BROWN,0.35f));
		}
	}
}

void draw_features(void){

	//red squares
	uint64_t tmp_red_sq = red_squares;
	while(tmp_red_sq){
		int sq = pop_lsb(tmp_red_sq);
		Vector2 pos = sq_to_coord(sq);

		if(!settings[TURN])
			pos = invert_coord(pos);
					
		int top=corner.y+pos.y*squaresize;
		int left=corner.x+pos.x*squaresize;
		DrawRectangle(left, top, squaresize, squaresize, Fade(MAROON, 0.45f));
		
	}

	//arrows
	for(auto [start,end]:arrows){

		if(!settings[TURN]){ 
			start = invert_coord(start); 
			end = invert_coord(end); 
		}
		drawArrow(start,end);
	}

	if(current_legality.checkers != 0ULL){//RED RING IF KING IS IN CHECK
		int sq = current_legality.ksq;
		Vector2 pos = sq_to_coord(sq);

		if(!settings[TURN])
			pos = invert_coord(pos);

		Vector2 center;
		center.x = corner.x + squaresize*pos.x + squaresize/2.0f;
		center.y = corner.y + squaresize*pos.y + squaresize/2.0f;

		DrawRing(center, squaresize*0.38f, squaresize*0.45f, 0, 360, 36, Fade(MAROON, 0.6f));
	}

	//held piece

	if(held_piece != EMPTY){
		Vector2 pos = GetMousePosition();
		pos.x-=texall[currentp.w_to_move][held_piece].width/2.0f*scalepiece;
		pos.y-=texall[currentp.w_to_move][held_piece].height/2.0f*scalepiece;
		draw_piece(currentp.w_to_move,held_piece, pos);

		draw_attack_table(find_moves(currentp, current_legality, held_piece, held_piece_sq));

	}

	if(settings[EVAL])eval_bar_draw();

	take_back_button.button_drawer();

	restart_button.button_drawer();

	lobby_button.button_drawer();
}

