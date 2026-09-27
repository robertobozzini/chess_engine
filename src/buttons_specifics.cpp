#include "buttons_specifics.h"

using namespace std;

//deve avere accesso alle impostazioni
void start_button(void){
    //inizializza i bottoni specifici
    //Va alla sezione 2 : impostazioni
    menu_section = 1;
    return;
}

void play_button(void){ //=0
    //inizializza i bottoni specifici
    change_section = true;
    //va al game
    return;
}

void turn_button(void){ //=1
    //inizializza i bottoni specifici
    int pos = 1;
    settings[pos] = !settings[pos];
    return;
}

void bot_button(void){ //=2
    //inizializza i bottoni specifici
    int pos = 2;
    settings[pos] = !settings[pos];
    return;
}

void eval_button(void){ //=3
    //inizializza i bottoni specifici
    int pos = 3;
    settings[pos] = !settings[pos];
    return;
}

void audio_button(void){ //=4
    //inizializza i bottoni specifici
    int pos = 4;
    settings[pos] = !settings[pos];
    return;
}