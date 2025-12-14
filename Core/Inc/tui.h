#ifndef H_TUI
#define H_TUI

#define ENTER		10
#define BACKSPACE	127
#define ESCAPE		27
#define UP_ARROW 	65
#define DOWN_ARROW	66 
#define RIGHT_ARROW	67
#define LEFT_ARROW	68

#define MAX_BUF		2048

#define CLEAR_DISPLAY L"\033[2J\033[H"

#define CURSOR_STYLE "\033[2 q"
#define CURSOR_SHOW "\033[?25h"
#define CURSOR_HIDE "\033[?25l"

#define BLOCK L"\033[47m \033[0m"

#include "menu.h"
#include <sys/ioctl.h>

void moveCursor(int, int);
void set_noncanonical_mode(int);

void handle_signal(int);

struct Result menu_switch(struct menu_t**, struct winsize*);
struct Result menu_enter(struct menu_t**, struct winsize*);
struct Result menu_r_arrow(struct menu_t**, struct winsize*);
struct Result menu_l_arrow(struct menu_t**, struct winsize*);

struct Result prompt_switch(struct menu_t*, struct winsize*);
struct Result init_prompt(struct menu_t*, struct winsize*);

struct Result draw_next_menu(struct menu_t**, struct winsize*);

#endif
