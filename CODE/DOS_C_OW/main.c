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
#define int_video_set_cursor_pos 0x02
#define int_video_get_cursor_pos 0x03
#define int_video_get_mode 0x0F

#define KEY_ESC 27
#define KEY_ENT 13

// Globals enums
enum {
    GS_GAME,
    GS_TITLE,
    GS_DEBRIEF,
    GS_EXIT
} gamestate_t;

// Global Struct definitions
//struct {
//
//} entity_t;

// Globals
unsigned int lfsr; // Current random number
char orig_video_mode;
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

// Mimics BASIC locate, 1 indexed
void basic_locate(char row, char col){
    reg_pack->h.ah = int_video_get_cursor_pos;
    intr(int_video,reg_pack); // Sets active page
    reg_pack->h.ah = int_video_set_cursor_pos;
    reg_pack->h.dh = row - 1;
    reg_pack->h.dl = col - 1;
    intr(int_video,reg_pack);
}

// Random function, does not simulate BASIC's and instead implements a linear feedback shift register
// Input: 0, to get the existing random number again
//        1, to get a new random number
//        -1 to -32768 to re-seed
unsigned int basic_random(int x){
    if(x == 0) {// just return last value
        return lfsr;
    }
    else if ( x < 0 ){ // New Seed, needs to be less than
        lfsr=-1*x;
        printf("Seeding with: %du\r\n",lfsr);
    }
    // Calculate new random number
    lfsr = ((((lfsr >> 0)^(lfsr >> 2)^(lfsr >> 3)^(lfsr >> 5)) & 0x01) << 15) | (lfsr >> 1); // x^16 + x^14 + x^13 + x^11 + 1
    return lfsr;
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
    basic_locate(10,1);
    basic_print("DROP: PCjr/C/OpenWatcom/v1");
    basic_locate(11,5);
    basic_print("By: Ryan Paterson");
    basic_locate(15,1);
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

unsigned long long int period;
unsigned int start_state; // must be less than 16768
unsigned int current_state;

// --- CODE STARTS HERE ---
int main(){
    // Save video mode off
    reg_pack->h.ah = int_video_get_mode;
    intr(int_video,reg_pack);
    orig_video_mode = reg_pack->h.al;

    // Start the game already
    gamestate=GS_TITLE;
    game_loop();

    // Restore the video mode
    reg_pack->w.ax = (int_video_set_mode) << 8 | orig_video_mode;
    intr(int_video,reg_pack);
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
