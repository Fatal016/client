#include <stdbool.h>
#include <stddef.h>

#include "../Inc/menu.h"
#include "../Inc/templates.h"

struct column_t search_menu_search_column = {
	ROWS (
		&(struct row_t) {
			.data = &(struct prompt_t) {
				.name = "Search:",
				.placeholder = "<Search Movies>",
				.buf_pos = 0
				
			},
			.type = PROMPT
		}
	),

	COLUMN_DEFAULTS
};

struct column_t search_menu_results_column = {
	.rs = NULL,

	COLUMN_DEFAULTS
};

struct menu_t search_menu = {

	.pm = &main_menu,

	.pn = "Search",

	COLS (
		&search_menu_search_column,
		&search_menu_results_column
	),

	MENU_DEFAULTS
};

struct column_t main_menu_navigation_column = {
	ROWS (
		&(struct row_t){
			.data = &search_menu,
			.type = MENU,
			ROW_DEFAULTS
		}
	),

	COLUMN_DEFAULTS
};

struct menu_t main_menu = {

	.pm = NULL,

	.pn = NULL,

	COLS (
		&main_menu_navigation_column
	),

	MENU_DEFAULTS
};


/*
struct menu_t *category_menu_items[] = {
	&torrent_info_menu,
	&tracker_info_menu,
	&meta_info_menu,
	&files_menu
};

struct menu_t category_menu = {
	.pretty_name 	= NULL,
	
	.items 			= (void *)category_menu_items,
	.type 			= MENU,
	.prev_menu 		= NULL,

	.ref_x 			= 1,
	.ref_y 			= 1,
	
	.size_x			= 1,
	.size_y 		= 4,
	
	.cur_x 			= 1,
	.cur_y 			= 1
};


struct field_t **torrent_info_menu_items;
struct menu_t torrent_info_menu = {
	.pretty_name 	= L"Torrent Info",
	
	.items 			= NULL,
	.type 			= FIELD,
	.prev_menu 		= &category_menu,
	
	.ref_x 			= 1,
	.ref_y 			= 1,
	
	.size_x			= 1,
	.size_y 		= 2,
	
	.cur_x 			= 1,
	.cur_y 			= 1
};
struct field_t torrent_info_menu_items_template[] = {
	{L"Filename :", NULL},
	{L"Info Hash:", NULL}
};

struct field_t **tracker_info_menu_items;
struct menu_t tracker_info_menu = {
	.pretty_name 	= L"Tracker Info",

	.items 			= NULL,
	.type 			= FIELD,
	.prev_menu 		= &category_menu,
	
	.ref_x 			= 1,
	.ref_y 			= 1,
	
	.size_x			= 1,
	.size_y 		= 0,
	
	.cur_x 			= 1,
	.cur_y 			= 1
};


struct field_t **meta_info_menu_items;
struct menu_t meta_info_menu = {
	.pretty_name	= L"Meta Info",

	.items			= NULL,
	.type			= FIELD,
	.prev_menu		= &category_menu,
	
	.ref_x			= 1,
	.ref_y			= 1,
	
	.size_x			= 1,
	.size_y			= 5,

	.cur_x			= 1,
	.cur_y			= 1
};
struct field_t meta_info_menu_items_template[] = {
	{L"Name        :", NULL},
	{L"Created On  :", NULL},
	{L"Created By  :", NULL},
	{L"Comment     :", NULL},
	{L"Piece Length:", NULL},
//	{L"Private     :", NULL}
};

struct field_t **files_menu_items;
struct menu_t files_menu = {
	.pretty_name	= L"Files",

	.items			= NULL,
	.type			= FIELD,
	.prev_menu		= &category_menu,

	.ref_x			= 1,
	.ref_y			= 1,

	.size_x			= 1,
	.size_y			= 1,

	.cur_x			= 1,
	.cur_y			= 1
};
*/
