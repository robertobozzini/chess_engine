#include "buttons_specifics.h"

using namespace std;

//deve avere accesso alle impostazioni
void start_button(void){
    //inizializza i bottoni specifici
    //Va alla sezione 2 : impostazioni
    start_button_obj.state = 0;
    menu_section = 1;
    return;
}

void play_button(void){ //=0
    //inizializza i bottoni specifici
    menu_buttons[PLAY].state = 0;
    change_section = true;
    //va al game
    return;
}

void turn_button(void){ //=1
    //inizializza i bottoni specifici
    settings[TURN] = !settings[TURN];
    if(!settings[TURN])
        menu_buttons[TURN].text = "Playing as Black";
    else
        menu_buttons[TURN].text = "Playing as White";
    return;
}

void bot_button(void){ //=2
    //inizializza i bottoni specifici
    settings[BOT] = !settings[BOT];
    if(!settings[BOT])
        menu_buttons[BOT].text = "Playing with a friend";
    else
        menu_buttons[BOT].text = "Playing with a bot";
    return;
}

void eval_button(void){ //=3
    //inizializza i bottoni specifici
    settings[EVAL] = !settings[EVAL];
    if(!settings[EVAL])
        menu_buttons[EVAL].text = "Evaluation bar not active";
    else
        menu_buttons[EVAL].text = "Evaluation bar active";
    return;
}

void audio_button(void){ //=4
    //inizializza i bottoni specifici
    settings[AUDIO] = !settings[AUDIO];
    if(!settings[AUDIO])
        menu_buttons[AUDIO].text = "Audio not active";
    else
        menu_buttons[AUDIO].text = "Audio active";
    return;
}