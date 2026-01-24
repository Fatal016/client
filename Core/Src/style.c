#include "../Inc/style.h"

struct border_style BORDER_SINGLE = {
	.top_left = "\u250C",
	.top_right = "\u2510",
	.bottom_left = "\u2514",
	.bottom_right = "\u2518",
	.horizontal = "\u2500",
	.vertical = "\u2502",
	.top_junction = "\u252C",
	.left_junction = "\u2524",
	.center_junction = "\u253C",
	.right_junction	= "\u251C",
	.bottom_junction = "\u2534"
};

struct text_style BLACK_AND_WHITE = {
	.foreground = 30,
	.background = 47
};
