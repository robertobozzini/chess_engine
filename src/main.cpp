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



void loader(void){
    //chiama i loader vari delle sezioni
}

Vector2 mouse_position;

bool section = false;//false = menu, true = game

void generic_event_handler(void){
    //gestisce gli eventi per sezioni
    //+ eventi globali (tipo mouse position)
    

    mouse_position = GetMousePosition();

    if (event_handler == nullptr)
        return;
    
    bool change_section = event_handler();

    if (change_section == true){

        if(section == false)
            event_handler = game_event_handler;
        else
            event_handler = menu_event_handler;

        section = !section;
    }
    
}


void generic_draw(void){
    if (screen_drawer != nullptr)
        screen_drawer();
    
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

    loader();

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