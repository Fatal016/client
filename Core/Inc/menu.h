#ifndef H_MENU
#define H_MENU

//#include "tui.h"
#include <stdbool.h>
#include <sys/ioctl.h>

#define TOP_LEFT_CORNER 	0x250C
#define TOP_RIGHT_CORNER	0x2510
#define BOTTOM_LEFT_CORNER	0x2514
#define BOTTOM_RIGHT_CORNER	0x2518

#define TOP_JUNCTION		0x252C
#define BOTTOM_JUNCTION		0x2534
#define LEFT_JUNCTION		0x2524
#define RIGHT_JUNCTION		0x251C
#define T_JUNCTION		0x253C

#define VERTICAL_BAR		0x2502
#define HORIZONTAL_BAR		0x2500

struct Result {
        int rc;
        char* msg;
        void* data;
};

enum row_type {
	MENU,
	FIELD,
	PROMPT,
	BREAK
};

enum side {
	LEFT,
	RIGHT
};

enum prompt_mode {
	TRAVERSE,
	ENTRY
};

enum column_profile {
	MIN,
	SPLIT,
	MAX
};

struct row_t {
	void *data;
	enum row_type type;

	int rx;
	bool rx_set;

	int ry;
	bool ry_set;
}; 

struct prompt_t {
	char *name;
	char *value;
	char *placeholder;

	enum prompt_mode mode;
	int buf_pos;

	int height;

	int name_len;
	int value_len;
	int len;
};

struct field_t {
	char *name;
	char *value;
	int len;
};


struct column_t {

	/* (r)ow(s) */
	struct row_t **rs;
	
	/* (n)um (r)ows */
	size_t nr;

	/* (c)urrent (r)ow */
	int cr;
	bool cr_set;

	/* MUST BE RELATIVE TO PARENT MENU */

	/* (r)eference window position (x) */
	int rx;
	bool rx_set;

	/* (r)eference window position (y) */
	int ry;
	bool ry_set;

	/* (s)ize (x) */
	int sx;
	bool sx_set;
	
	/* (s)ize (y) */
	int sy;
	bool sy_set;
};

struct menu_t {
	
	/* (p)revious (m)enu */
	struct menu_t *pm;
	
	/* (p)retty (n)ame */
	char *pn;
	
	/* (c)olumn(s) */
	struct column_t **cs;

	/* (n)um (c)olumns */
	size_t nc;

	/* (c)urrent (c)olumn */
	int cc;
	bool cc_set;
	
	/* (r)eference window position (x) */
	int rx;
	bool rx_set;

	/* (r)eference window position (y) */
	int ry;
	bool ry_set;

	/* (s)ize (x) */	
	int sx;
	bool sx_set;

	/* (s)ize (y) */
	int sy;
	bool sy_set;

	enum column_profile cp;
	bool cp_set;

	int len;
};

int max_size(struct menu_t*);

//int resize_menu(struct menu_t*);

//struct Result draw_box(int, int, int, int);
struct Result draw_box_general(int, int, int, int);
struct Result draw_box_aware(struct menu_t*, int, int, int, int);

struct Result draw_vertical_bar(int, int, int);


struct Result draw_module(struct menu_t*, struct winsize*);
struct Result draw_column(struct menu_t*, struct winsize*, int);
struct Result draw_row(struct menu_t*, struct winsize*, int, int);

struct Result draw_menu(struct menu_t*);
struct Result draw_field(struct field_t*);
struct Result draw_prompt(struct prompt_t*);


int clear_style(struct menu_t*, struct winsize*);

struct Result prompt_style(struct menu_t*, struct winsize*, enum prompt_mode);
struct Result set_style(struct menu_t*, struct winsize*);

struct Result init_module(struct menu_t*, struct winsize*);
struct Result scale_module(struct menu_t*, struct winsize*);

struct Result draw_column_dividers(struct menu_t*);
struct Result draw_dividers(struct menu_t*);

struct Result draw_horizontal_bar(int, int, int);

#endif
