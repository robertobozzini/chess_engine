#include "main.h"
#include "menu_main.h"
#include "game_main.h"

using namespace std;

//dimensioni schermo di base
int ScreenWidth = 1200;
int ScreenHeight = 800;

bool (*event_handler)(void);//coso figo
void (*screen_drawer)(void);

//cosi posso chiamare sempre la stessa funzione (che è un pointer--
//alla funzione che voglio usare in quel momento

bool in_game = false;//false = menu, true = game


Vector2 mouse_position;

Texture2D cursor_base;
Texture2D cursor_hold;

bool holding_cursor = false;

float scale_cursor_base = 0.09f;
float scale_cursor_hold = 0.07f;

void init(void){
    //chiama i loader vari delle sezioni (texture, font, ecc)
    // chiama gli init delle sezioni
    event_handler = menu_event_handler;
    screen_drawer = menu_drawer;

    cursor_base = LoadTexture("../images/CursorBase.png");
	cursor_hold = LoadTexture("../images/CursorHolding.png");

    menu_init();
    game_init();
}

void generic_event_handler(void){
    //gestisce gli eventi per sezioni
    //+ eventi globali (tipo mouse position)
    

    mouse_position = GetMousePosition();
    holding_cursor = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

    if (event_handler == nullptr)
        return;
    
    bool change_section = event_handler();

    if (change_section == true){
        in_game = !in_game;

        if(in_game == true){

            event_handler = game_event_handler;
            screen_drawer = game_drawer;
        }
        else {

            event_handler = menu_event_handler;
            screen_drawer = menu_drawer;
        }
    }
    
}


void generic_draw(void){
    if (screen_drawer != nullptr)
        screen_drawer();

    float scale_cursor = holding_cursor ? scale_cursor_hold : scale_cursor_base;
    mouse_position.x -= cursor_base.width * scale_cursor/2;
    mouse_position.y -= cursor_base.height * scale_cursor/2;

    DrawTextureEx(
        holding_cursor ? cursor_hold : cursor_base, 
        mouse_position, 
        0.0f, 
        scale_cursor, 
        WHITE
    );
}

int main(void)
{
    //inizializzo la finestra
    InitWindow(ScreenWidth, ScreenHeight, "Chess Engine");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    //inizializzo il pointer alla funzione di disegno
    screen_drawer = nullptr;
    event_handler = nullptr;

    init();

    while (!WindowShouldClose())
    {
        //gestione eventi
        generic_event_handler();


        //disegno
        BeginDrawing();

            generic_draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}