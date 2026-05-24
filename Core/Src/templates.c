#include <stdbool.h>
#include <stddef.h>

#include "../Inc/menu.h"
#include "../Inc/templates.h"

struct column search_menu_search_column = {
	ROWS (
		&(struct row) {
			.data = &(struct field) {
				.name = "Search",
				.placeholder = "<Search Movies>",
				.value_pos = 0,
				.read_only = false,
				
				.min_traverse_height = 1,
				.max_traverse_height = 1,

				.min_entry_height = 1,
				.max_entry_height = 0
			},
			.type = FIELD,
		},
		&(struct row) {
			.data = &(struct field) {
				.name = "Test",
				.placeholder = "<Placeholder>",
				.value_pos = 0,
				.read_only = false,

				.min_traverse_height = 1,
				.max_traverse_height = 1,

				.min_entry_height = 1,
				.max_entry_height = 2
			},
			.type = FIELD,
		}
	),

	COLUMN_DEFAULTS,

	.cr = 0
};

struct column search_menu_null_column = {
	ROWS (
		&(struct row) {
			.type = BREAK
		},
		&(struct row) {
			.type = BREAK
		},
		&(struct row) {
			.type = BREAK
		},
		&(struct row) {
			.type = BREAK
		},
		&(struct row) {
			.type = BREAK
		},
		&(struct row) {
			.type = BREAK
		},
		&(struct row) {
			.type = BREAK
		},
		&(struct row) {
			.type = BREAK
		},
		&(struct row) {
			.type = BREAK
		},
		&(struct row) {
			.type = BREAK
		}
	),

	COLUMN_DEFAULTS
};

struct column search_menu_results_column = {
	ROWS (
		&(struct row) {
			.type = BREAK
		},
		&(struct row) {
			.type = BREAK
		},
		&(struct row) {
			.type = BREAK
		},
		&(struct row) {
			.type = BREAK
		},
		&(struct row) {
			.type = BREAK
		},
		&(struct row) {
			.type = BREAK
		},
		&(struct row) {
			.type = BREAK
		},
		&(struct row) {
			.type = BREAK
		},
		&(struct row) {
			.type = BREAK
		},
		&(struct row) {
			.type = BREAK
		}
	),

	COLUMN_DEFAULTS
};

struct menu search_menu = {

	.pm = &main_menu,

	.pn = "Search",

	COLS (
		&search_menu_null_column,
		&search_menu_search_column,
		&search_menu_results_column
	),

	MENU_DEFAULTS,

	.cc = 1,
	.cc_set = true
};

struct column main_menu_navigation_column = {
	ROWS (
		&(struct row){
			.data = &search_menu,
			.type = MENU,
			ROW_DEFAULTS
		}
	),

	COLUMN_DEFAULTS
};

struct menu main_menu = {

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
