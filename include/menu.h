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
	BREAK
};

enum field_mode {
	TRAVERSE,
	ENTRY
};

enum column_profile {
	MIN,
	SPLIT,
	MAX
};

// Am I going with signals?
struct pending {
	bool init;
	bool scale;
};

struct row {
	struct menu *menu;
	struct column *column;

	int index;

	void *data;
	enum row_type type;

	int rx;
	bool rx_set;

	int ry;
	bool ry_set;

	int sx;
	int sy;
}; 

struct field {
	struct menu *menu;
	struct column *column;
	struct row *row;

	char *name;
	char *placeholder;
	bool read_only;

	char *value;
	size_t value_pos;

	int name_len;
	size_t value_len;

	int value_offset;

	// Probably don't need this to be field bound since you're bound to entry
	// if you're doing it	
	//enum field_mode mode;
	
	int min_entry_height;
	int max_entry_height;

	int min_traverse_height;
	int max_traverse_height;

	size_t max_entry_length;
};

struct column {

	struct menu *menu;

	int index;

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
struct Result draw_box_aware(struct row*, int, struct style *s);

struct Result draw_vertical_bar(int, int, int, struct style*);


struct Result draw_module(struct menu*, struct winsize*, struct style*);
struct Result draw_column(struct column *c, struct style*);
struct Result draw_row(struct row *r, struct style*);

struct Result draw_menu(struct menu*, struct style*);
struct Result draw_field(struct field*, struct style*, enum field_mode);

int clear_style(struct menu*, struct winsize*);

struct Result set_style(struct row*, struct style *s);

struct Result set_style_field(struct field*, enum field_mode, struct style *s);

struct Result init_module(struct menu*, struct winsize*);
struct Result scale_module(struct menu*, struct winsize*);

struct Result draw_column_dividers(struct menu*, struct style *s);
struct Result draw_dividers(struct menu*, struct style *s);

struct Result draw_horizontal_bar(struct column*, int, struct style*);
struct Result clear_horizontal_bar(struct column*, int, struct style*);

bool edge_detect(struct column*, int);

struct Result clear_box(struct menu *m, struct style *s);

struct Result clear_field_box(struct field *f, struct style *s, enum field_mode fm);

struct Result init_field(struct field *f, struct row *cr);

struct Result unset_style(struct row *r, struct style *s);
struct Result unset_style_field(struct field *f, struct style *s);

#endif
