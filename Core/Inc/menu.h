#ifndef H_MENU
#define H_MENU

//#include "tui.h"
#include <stdbool.h>
#include <sys/ioctl.h>

#include "style.h"

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

struct pending {
	bool init;
	bool scale;
};

struct row {
	void *data;
	enum row_type type;

	int rx;
	bool rx_set;

	int ry;
	bool ry_set;

	int height;
	int height_set;
}; 

struct prompt {
	char *name;
	char *value;
	char *placeholder;

	enum prompt_mode mode;
	size_t buf_pos;

	int cr;
	int cr_set;

	int height;
	int height_set;

	int name_len;
	int value_len;
	int len;
};

struct field {
	char *name;
	char *value;
	int len;
};


struct column {

	/* (r)ow(s) */
	struct row **rs;
	
	/* (n)um (r)ows */
	int nr;

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

struct menu {
	
	/* (p)revious (m)enu */
	struct menu *pm;
	
	/* (p)retty (n)ame */
	char *pn;
	
	/* (c)olumn(s) */
	struct column **cs;

	/* (n)um (c)olumns */
	int nc;

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

	bool pending_init;
	bool pending_scale;
	bool pending_box;
	bool pending_dividers;
};

int max_size(struct menu*);

//int resize_menu(struct menu_t*);

//struct Result draw_box(int, int, int, int);
struct Result draw_box_general(int, int, int, int, struct style *s);
struct Result draw_box_aware(struct menu*, struct style *s, int, int, int, int);

struct Result draw_vertical_bar(int, int, int, struct style*);


struct Result draw_module(struct menu*, struct winsize*, struct style*);
struct Result draw_column(struct menu*, struct winsize*, struct style*, int);
struct Result draw_row(struct menu*, struct winsize*, struct style*, int, int);

struct Result draw_menu(struct menu*, struct style*);
struct Result draw_field(struct field*, struct style*);
struct Result draw_prompt(struct prompt*, struct style*);

int clear_style(struct menu*, struct winsize*);

struct Result set_style(struct menu*, struct winsize*, struct style *s);
struct Result prompt_style(struct menu*, struct winsize*, struct style *s, enum prompt_mode);

struct Result init_module(struct menu*, struct winsize*);
struct Result scale_module(struct menu*, struct winsize*);

struct Result draw_column_dividers(struct menu*, struct style *s);
struct Result draw_dividers(struct menu*, struct style *s);

struct Result draw_horizontal_bar(int, int, int, struct style*);

#endif
