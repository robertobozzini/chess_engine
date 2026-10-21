
//import particolari:
#include "main.h"
#include "lobby.h"
#include "chessboard.h"

using namespace std;

int ScreenWidth = 1200;
int ScreenHeight = 800;

/*
cambioprogramma: rettangolo
*/

void (*event_handler)(Status *status);//coso figo
void (*screen_drawer)(Status *status);

//cosi posso chiamare sempre la stessa funzione (che è un pointer--
//alla funzione che voglio usare in quel momento



Texture2D cursorb;
Texture2D cursorh;

float cursorscaleb = 0.09f;
float cursorscaleh = 0.07f;
float cursorscale;

int main(void){
	
	Status status;

//	status.width = ScreenWidth;
//	status.height = ScreenHeight;
	
	InitWindow(ScreenWidth, ScreenHeight, "chess");
	//la scacchiera è quadrata
	
	SetExitKey(KEY_NULL);
	
	SetTargetFPS(60);
	
	event_handler=lobby_handler;
	screen_drawer=lobby_drawer;
	
	//main init:
	cursorb = LoadTexture("../images/CursorBase.png");
	cursorh = LoadTexture("../images/CursorHolding.png");
	lobby_init(&status);
	chessboard_init(&status);
	
	HideCursor();
	
	while(!WindowShouldClose()){
		
		event_handler(&status);
		
//		if(status.ingame){//entri in partita
//			//chessboard_init(&status);
//			event_handler=chessboard_handler;
//			screen_drawer=chessboard_drawer;
//		}
//		else {
//			//lobby_init(&status);
//			event_handler=lobby_handler;
//			screen_drawer=lobby_drawer;
//		}
			
		//status.isholding = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
		
		BeginDrawing();
			
			screen_drawer(&status);
		
			Vector2 mousepos = GetMousePosition();
		
			cursorscale =(status.isholding)?cursorscaleh : cursorscaleb;
		
			mousepos.x -= cursorb.width*cursorscale/2;
			mousepos.y -= cursorb.height*cursorscale/2;
		
			DrawTextureEx(
				(status.isholding)? cursorh : cursorb,
				mousepos,
				0,
				cursorscale,
				WHITE
			);
		
		EndDrawing();
		
	}
	
	CloseWindow();
}
