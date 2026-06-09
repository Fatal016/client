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

	char *text_divider;
	int text_divider_len;

	int border_padding;
	int border_padding_left;
	int border_padding_right;
};

extern struct border_style BORDER_SINGLE;

extern struct text_style BLACK_AND_WHITE;

struct style *style_init(struct border_style*, struct text_style*, char *text_divider, int, int);

#endif
