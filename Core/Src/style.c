#include <string.h>
#include <stdlib.h>

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

struct style *style_init(struct border_style *border,
			 struct text_style *text,
			 char *text_divider,
			 int border_padding_left,
			 int border_padding_right)
{
	struct style *s = malloc(sizeof(struct style));
	s->border = border;
	s->text = text;
	s->text_divider = text_divider;
	s->text_divider_len = strlen(text_divider);

	s->border_padding_left = border_padding_left,
	s->border_padding_right = border_padding_right,
	s->border_padding = border_padding_left + border_padding_right;

	return s;
}
