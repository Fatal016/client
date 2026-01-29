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

#define CLEAR_DISPLAY "\033[2J\033[H"

#define CURSOR_STYLE "\033[2 q"
#define CURSOR_SHOW "\033[?25h"
#define CURSOR_HIDE "\033[?25l"

#define BLOCK "\033[47m \033[0m"

#include "menu.h"
#include <stdint.h>
#include <sys/ioctl.h>
#include "style.h"


void move(int, int);
void set_noncanonical_mode(int);

void handle_signal(int);


struct Result menu_switch(
	struct menu **menu,
	struct winsize *winsize,
	struct style *style
);
struct Result menu_r_arrow(
	struct menu **menu,
	struct winsize *winsize,
	struct style *style
);
struct Result menu_l_arrow(
	struct menu **menu,
	struct winsize *winsize,
	struct style *style
);
struct Result menu_enter(
	struct menu **menu,
	struct winsize *winsize,
	struct style *style
);

struct Result prompt_switch(
	struct menu *menu,
	struct winsize *winsize,
	struct style *style
);
struct Result prompt_r_arrow(
	struct menu *menu,
	struct winsize *winsize,
	struct style *style
);
struct Result prompt_l_arrow(
	struct menu *menu,
	struct winsize *winsize,
	struct style *style
);
struct Result prompt_enter(
	struct menu *menu,
	struct winsize *winsize,
	struct style *style
);
struct Result prompt_backspace(
	struct menu *menu,
	struct winsize *winsize,
	struct style *style
);
struct Result prompt_escape(
	struct menu *menu,
	struct winsize *winsize,
	struct style *style
);


struct Result init_prompt(struct menu*, struct winsize*, struct style *s);

struct Result prompt_char(struct menu*, struct winsize*, int*);

struct Result draw_next_menu(struct menu**, struct winsize*, struct style *);

void clear_column(struct menu*);

struct Result set_edge_vertical_bar(struct menu*, struct style *, const int *h, struct edges *);
struct Result set_edge_vertical_bar_left(struct menu*, struct style *s, const int *h, struct edges *);

struct Result set_edge_right_junction(struct menu*, struct style *, const int *h, struct edges *);
struct Result set_edge_left_junction(struct menu*, struct style *, const int *h, struct edges *);
struct Result set_edge(struct menu*, struct style *, const int *h, const char *);


struct Result utf8_encode(uint32_t, char*);
uint32_t utf8_decode(const char *c);
//void print_utf8(uint32_t);

#endif
