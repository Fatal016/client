#ifndef H_TUI
#define H_TUI

#define ENTER		10
#define UP_ARROW 	65
#define DOWN_ARROW	66 
#define RIGHT_ARROW	67
#define LEFT_ARROW	68

#define CLEAR_DISPLAY L"\033[2J\033[H"
#define HIDE_CURSOR L"\033[?25l"

#include "menu.h"
#include <sys/ioctl.h>

void moveCursor(int, int);
void set_noncanonical_mode(void);

void handle_signal(int);

struct Result menu_switch(struct menu_t**, struct winsize*);
struct Result menu_enter(struct menu_t**, struct winsize*);
struct Result menu_r_arrow(struct menu_t**, struct winsize*);
struct Result menu_l_arrow(struct menu_t**, struct winsize*);


struct Result init_prompt(struct menu_t*, struct winsize*);

#endif
