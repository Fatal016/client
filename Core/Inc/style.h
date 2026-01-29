#ifndef H_STYLE
#define H_STYLE

struct border_style {
	char *top_left;
	char *top_right;
	char *bottom_left;
	char *bottom_right;
	char *horizontal;
	char *vertical;
	char *top_junction;
	char *left_junction;
	char *center_junction;
	char *right_junction;
	char *bottom_junction;
};

struct text_style {
	const int foreground;
	const int background;
};

struct style {
	struct border_style *border;
	struct text_style *text;
};

extern struct border_style BORDER_SINGLE;

extern struct text_style BLACK_AND_WHITE;

#endif
