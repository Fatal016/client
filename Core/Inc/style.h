#ifndef H_STYLE
#define H_STYLE

struct border_style {
	const char *top_left;
	const char *top_right;
	const char *bottom_left;
	const char *bottom_right;
	const char *horizontal;
	const char *vertical;
	const char *top_junction;
	const char *left_junction;
	const char *center_junction;
	const char *right_junction;
	const char *bottom_junction;
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
