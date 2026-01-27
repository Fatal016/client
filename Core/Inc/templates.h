#ifndef H_TEMPLATES
#define H_TEMPLATES

#include "menu.h"

#define ROWS(...) \
.rs = (struct row *[]) { __VA_ARGS__ }, \
.nr = sizeof((struct row *[]) { __VA_ARGS__ }) / sizeof(struct row *)

#define COLS(...) \
.cs = (struct column *[]) { __VA_ARGS__ }, \
.nc = sizeof((struct column *[]) { __VA_ARGS__ }) / sizeof(struct column *)

#define ROW_DEFAULTS		\
	.rx_set = false, 	\
	.ry_set = false,	\
	.height_set = false

#define COLUMN_DEFAULTS		\
	.rx_set = false, 	\
	.ry_set = false,	\
	.sx_set = false,	\
	.sy_set = false

#define MENU_DEFAULTS		\
	.cc_set = false,	\
	.rx_set = false,	\
	.ry_set = false,	\
	.sx_set = false,	\
	.sy_set = false,	\
	.cp_set = false

extern struct column search_menu_search_column;
extern struct column search_menu_results_column;
extern struct menu search_menu;

extern struct column main_menu_navigation_column;
extern struct menu main_menu;

//extern struct menu_t category_menu;
//extern struct menu_t *category_menu_items[];

//extern struct menu_t torrent_info_menu;
//extern struct field_t torrent_info_menu_items_template[];
//extern struct field_t **torrent_info_menu_items;

//extern struct menu_t tracker_info_menu;
//extern struct field_t **tracker_info_menu_items;

//extern struct menu_t meta_info_menu;
//extern struct field_t meta_info_menu_items_template[];
//extern struct field_t **meta_info_menu_items;

//extern struct menu_t files_menu;
//extern struct field_t **files_menu_items;

#endif
