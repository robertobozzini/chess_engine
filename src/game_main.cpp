#include "game_main.h"
#include "board.h"
#include "menu_main.h"
using namespace std;


float logoingscale = 0.3f;

void game_init(){

    board_init();
}

bool game_event_handler(){
    if(IsKeyPressed(KEY_ESCAPE)){
        menu_section = 0;
        return true;
    }
    return false;
}

void game_drawer(){
	ClearBackground(BLACK);
	
	DrawTextureEx(
        logo,
        (Vector2){corner.y - (float)squaresize/4.0f,corner.y - (float)squaresize/4.0f},
        0,
        logoingscale,
        WHITE
    );

    board_draw();

}