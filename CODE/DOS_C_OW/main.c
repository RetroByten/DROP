// --- DROP ---
// Code starts at int main(), can search --- CODE STARTS HERE ---

// Standard C includes
#include <stdio.h>

// DOS / BIOS specific includes
#include <conio.h> // For reading the keyboard directly
#include <dos.h>

// NOTE: Look in i86.h [included when dos.h is included] for register structures and interrupt call definitions

// Definitions
#define int_video 0x10
#define int_video_set_mode 0x00
#define mode_4025_greyscale 0x00
#define mode_8025_greyscale 0x02

#define KEY_ESC 27
#define KEY_ENT 13

// Globals enums
enum {
    GS_GAME,
    GS_TITLE,
    GS_DEBRIEF,
    GS_EXIT
} gamestate_t;

// Globals
union REGPACK* reg_pack;
int gamestate;
char c;

// Utility Function definitions

// Mimics BASIC CLS (clear screen) by resetting video mode
void basic_cls(){
    reg_pack->w.ax = (int_video_set_mode) << 8 | mode_4025_greyscale;
    intr(int_video,reg_pack);
}

// Mimics BASIC print by appending CRLF
void basic_print(char* s){
    printf("%s%c%c",s,'\r','\n');
}


// Game Function definitions
int game_game(){
    basic_cls();
    basic_print("GAME");
    do {
        c=getch();
    } while (c != KEY_ESC);
    return GS_DEBRIEF;
}

int game_title(){
    basic_cls();
    basic_print("DROP: PCjr/C/OpenWatcom/v1");
    basic_print("By: Ryan Paterson");
    basic_print("");
    basic_print("Press Enter to DROP...");
    basic_print("Press ESC to END MISSION...");

    //char c = getch();  // seems like open watcom C also doesn't like variables being declared after a function starts executing... :(
    do {
        c=getch();
        switch(c){
            case KEY_ENT:
                return GS_GAME;
                break;
            case KEY_ESC:
                return GS_EXIT;
                break;
        }
    } while( c != KEY_ESC );
    return GS_EXIT;
}

int game_debrief(){
    basic_cls();
    basic_print("DEBRIEF");
    basic_print("Press ESC to return to the title screen");
    do {
        c=getch();
    } while (c != KEY_ESC);
    return GS_TITLE;
}

// Routine to run prior to exiting the game, eventually should also restore screen
int game_exit(){
    basic_cls();
    return GS_EXIT;
}

void game_loop(){
    while(gamestate != GS_EXIT){
        switch(gamestate){
            case GS_TITLE:
                gamestate=game_title();
                break;
            case GS_GAME:
                gamestate=game_game();
                break;
            case GS_DEBRIEF:
                gamestate=game_debrief();
                break;
            default: // All other cases including exit
                gamestate=GS_EXIT;
                break;
        }
    }

    // Now gamstate = GS_EXIT
    game_exit();
}


// --- CODE STARTS HERE ---
int main(){
    gamestate=GS_TITLE;
    game_loop();
    return 0;
}

/* Reference Hola Mundo - waits for enter key, changes screen mode/clears screan, prints Hola Mundo, waits for enter key, sets screen mode, exits
printf("Waiting for key entry\r\n");
    getc(stdin);

    reg_pack->h.ah=int_video_set_mode;
    reg_pack->h.al=mode_8025_greyscale;
    intr(int_video,reg_pack);

    printf("Hola Mundo!\r\nWaiting for key entry\r\n");
    getc(stdin); // Wait for input

    reg_pack->h.ah=int_video_set_mode;
    reg_pack->h.al=mode_4025_greyscale;
    intr(int_video,reg_pack);
*/
