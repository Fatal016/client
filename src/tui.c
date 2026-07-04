#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <string.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <stdint.h>

#include "../include/buffer.h"
#include "../include/tui.h"
#include "../include/style.h"

struct Result draw_next_menu(struct menu **m, struct winsize *w, struct style *s)
{
	struct Result result;

	struct column *cc = (*m)->cs[(*m)->cc];

	printf(CLEAR_DISPLAY);
	*m = (struct menu *)(cc->rs[cc->cr]->data);
	result = draw_module(*m, w, s);

	set_style(cc->rs[cc->cr], s);

	return result;

}

struct Result menu_r_arrow(struct menu **m, struct winsize *w, struct style *s)
{
	struct Result r;
/*
	struct column *cc = (*m)->cs[(*m)->cc];

	switch(cc->rs[cc->cr]->type) {
		case MENU:
			r = draw_next_menu(m, w);
		case FIELD:
			break;
		case PROMPT:
			break;
		default:
			break;
	}
*/
	r.rc = 0;
	return r;
}

struct Result menu_l_arrow(struct menu **m, struct winsize *w, struct style *s)
{
	struct Result r;
//	struct column_t *cc = (*m)->cs[(*m)->cc];
/*
	if ((*m)->pm != NULL) {
		wprintf(CLEAR_DISPLAY);
		*m = (*m)->pm;
		r = draw_module(*m, w);
		r = set_style(*m, w);
	}
*/
	r.rc = 0;
	return r;
}

struct Result menu_d_arrow(struct menu *m, struct style *s)
{
	struct Result r;

	struct column *cc = m->cs[m->cc];

	if (cc->cr == cc->nr - 1) {
		r.rc = 0;
		return r;
	}
	
	r = unset_style(cc->rs[cc->cr], s);

	if (cc->rs[cc->cr + 1]->type == BREAK) {
		if (cc->cr + 1 == cc-> nr - 1) {
			r.rc = 0;
			return r;
		} else {
			cc->cr++;
			r = menu_d_arrow(m, s);
		}
	} else {
		cc->cr++;
		r = set_style(cc->rs[cc->cr], s);
	}

	return r;
}

struct Result menu_u_arrow(struct menu *m, struct style *s)
{
	struct Result r;

	struct column *cc = m->cs[m->cc];

	if (cc->cr == 0) {
		r.rc = 0;
		return r;
	}

	r = unset_style(cc->rs[cc->cr], s);
	
	if (cc->rs[cc->cr - 1]->type == BREAK) {
		if (cc->cr + 1 == cc-> nr - 1) {
			r.rc = 0;
			return r;
		} else {
			cc->cr--;
			r = menu_u_arrow(m, s);
		}
	} else {
		cc->cr--;
		r = set_style(cc->rs[cc->cr], s);
	}

	return r;
}

struct Result menu_enter(struct menu **m, struct winsize *w, struct style *s)
{
	struct Result r;

	struct column *cc = (*m)->cs[(*m)->cc];
	struct row *cr = cc->rs[cc->cr];

	switch(cr->type) {
		case MENU:
			r = draw_next_menu(m, w, s);
			break;
		case FIELD:
			set_style_field(
				(struct field *)cr->data,
				(enum field_mode){ ENTRY }, s
			);
			break;
		default:
			break;
	}

	r.rc = 0;
	return r;
}


struct Result field_enter(struct field *f, struct style *s)
{
	struct Result r;

	buf[buf_len] = '\0';
	if (buf_len > 0) {
		if (f->value != f->placeholder) {
			if (f->value_len < buf_len) {
				f->value = realloc(f->value, buf_len * sizeof(char));
			}
		} else {
			f->value = malloc(buf_len * sizeof(char));
		}
		strncpy(f->value, buf, buf_len + 1);
	} else {
		if (f->value != f->placeholder) {
			free(f->value);
			f->value = NULL;
		}
	}

	f->value_pos = buf_pos;


/*
	r = set_edge(
		m,
		s,
		m->cc,
		
		s->border->vertical
	);


	move(cc->rx, cc->rs[cc->cr]->ry - 1);

	r = set_edge(
		m,
		s,
		s->border->vertical
	);
*/
	fflush(stdout);;

	r.rc = 0;
	return r;
}


struct Result field_escape(struct field *f, struct style *s)
{
	struct Result r;
	int c;

	c = getchar();
	// '[' -> ANSI/VT100
	// 'O' -> Application Mode
	if (c == '[' || c == 'O') {
		c = getchar();
		if (c == 'C') {
			if (buf_pos < buf_len) {
				buf_pos++;
				field_cursor_shift(f, s);

/*
				r = field_char_check_edge(f, s);
				if (r.rc) {
					move(
						f->column->rx + s->border_padding_left,
						f->row->ry + r.rc
					);
				}
				printf("\033[1C");
*/
			}
		} else if (c == 'D') {
			if (buf_pos > 0) {
//				r = field_char_check_edge(f, s);
				buf_pos--;
				printf("\b");

				field_cursor_shift(f, s);
/*
				if (r.rc) {
					move(
						f->row->rx + f->row->sx - 1,
						f->row->ry + r.rc - 1
					);
				} else {
					printf("\b");
				}
*/
			}

		}
	}

	r.rc = 0;
	return r;
}

struct Result field_char_check_edge(struct field *f, struct style *s)
{
	struct Result r;

	int num = (f->name_len + s->text_divider_len + buf_pos);
	int den = f->row->sx;

// Meed to have this let shift only occur after +1 to mod.
// Want to have cursor end of line for sake of limitations
/*
	// If right edge
	if (get_field_pos_mod(f, s) == f->row->sx) {
		num++;
//		num--;
	} else {
		num--;
	}
*/
	if (num % den == 0) {
		r.rc = num/den;
		return r;
	}

	r.rc = 0;
	return r;
}

void field_cursor_shift(struct field *f, struct style *s)
{
	if (f->value_offset <= 0) {
		// Ensures that cursor remains at end of line instead of jumping down
		if (get_field_pos_mod(f, buf_pos, s) == 0) {
			move(
				f->row->rx + f->row->sx,
				f->row->ry + get_field_pos_line(f, buf_pos, s) - 1
			);
		} else {
			move(
				f->row->rx + get_field_pos_mod(f, buf_pos, s),
				f->row->ry + get_field_pos_line(f, buf_pos, s)
			);
		}
	}
}


int get_field_height(struct field *f, struct style *s, enum field_mode fm)
{
	int h = 0;

	switch (fm) {
		case ENTRY:
			if (f->min_entry_height > get_field_len_line(f, f->value_len, s)) {
//			if (f->min_entry_height > get_field_len_line(f, buf_len, s)) {
				h = f->min_entry_height;
			} else {
				h = get_field_len_line(f, f->value_len, s);
//				h = get_field_len_line(f, buf_len, s);
			}
			break;
		case TRAVERSE:
			break;
	}

	return h;
}
/*
int get_field_pos(
	struct field *f,
	size_t *pos,
	struct style *s)
{
	return f->name_len + s->text_divider_len + *pos;
}
*/
int get_field_pos_line(
	struct field *f,
	size_t pos,
	struct style *s)
{
	return (f->name_len + s->text_divider_len + pos)/f->row->sx;
}

int get_field_pos_mod(
	struct field *f,
	size_t pos,
	struct style *s)
{
	return (f->name_len + s->text_divider_len + pos) % f->row->sx;
}

int get_field_len(
	struct field *f,
	size_t len,
	struct style *s)
{
	return f->name_len + s->text_divider_len + len;
}

int get_field_len_line(
	struct field *f,
	size_t len,
	struct style *s)
{
	if (!get_field_len_mod(f, len, s)) {
		return (f->name_len + s->text_divider_len + len)/f->row->sx;
	} else {
		return (f->name_len + s->text_divider_len + len)/f->row->sx + 1;
	}
}

int get_field_len_mod(
	struct field *f,
	size_t len,
	struct style *s)
{
	return (f->name_len + s->text_divider_len + len) % f->row->sx;
}

struct Result field_char_shift(
	struct field *f,
	char *buffer,
	size_t pos,
	size_t len,
	enum field_mode fm,
	struct style *s)
{
	struct Result r;

// Behaves differently depending on input/traverse
// Traverse will always want to print from the beginnging, traverse will not
// Switch case here?
/*
	switch(fm) {
		case TRAVERSE:
			break;
		case ENTRY:
			if (get_field_len(f, len, s) > f->max_entry_height * f->row->sx) {
				move(
					f->row->rx + f->name_len + s->text_divider_len,
					f->row->ry
				);
				printf("%s", "...");
			}
			break;
	};
*/

	// Need to incorporate some additional offset var here

	f->value_offset = get_field_len(f, len, s) - (f->row->sx * f->max_entry_height);

	// If char does not necessitate a shift
	// <= may or may not be correct here
	if (pos == len - 1 && f->value_offset <= 0) {
		move(
			f->row->rx + get_field_pos_mod(f, pos, s),
			f->row->ry + get_field_pos_line(f, pos, s)
		);
		printf("%c", buf[pos]);

		r.rc = 0;
		return r;
	}

	// This can't be line based, or at least has to recognize that current line can implicate first if there is overflow

	if (f->value_offset < 0) {
		for (int i = get_field_pos_line(f, pos, s); i < get_field_len_line(f, len, s); i++) {
			if (i == get_field_pos_line(f, pos, s)) {
				move(
					f->row->rx + get_field_pos_mod(f, pos, s),
					f->row->ry + get_field_pos_line(f, pos, s)
				);
				for (int j = 0; j < f->row->sx - get_field_pos_mod(f, pos, s); j++) {
					printf(" ");
				}
				move(
					f->row->rx + get_field_pos_mod(f, pos, s),
					f->row->ry + get_field_pos_line(f, pos, s)
				);

				printf(
					"%.*s",
					f->row->sx - get_field_pos_mod(f, pos, s),
					buffer + pos
					
				);
			} else {
				move(f->row->rx, f->row->ry + i);
				for (int j = 0; j < f->row->sx; j++) {
					printf(" ");
				} 
				move(f->row->rx, f->row->ry + i);
				printf(
					"%.*s",
					f->row->sx,
					buffer + (f->row->sx - f->name_len - s->text_divider_len) + f->row->sx * (i - 1)
				);
			}
		}
	} else {
		for (int i = 0; i < f->max_entry_height; i++) {
			if (i == 0) {
				move(
					f->row->rx + f->name_len + s->text_divider_len,
					f->row->ry
				);
				for (int j = 0; j < f->row->sx - f->name_len - s->text_divider_len; j++) {
					printf(" ");
				}
				move(
					f->row->rx + f->name_len + s->text_divider_len,
					f->row->ry
				);

				printf(
					"%.*s",
					f->row->sx - f->name_len - s->text_divider_len,
					buffer + f->value_offset
				);
			} else {
				move(
					f->row->rx,
					f->row->ry + i
				);
				for (int j = 0; j < f->row->sx; j++) {
					printf(" ");
				}
				move(
					f->row->rx,
					f->row->ry + i
				);
				printf(
					"%.*s",
					f->row->sx,
					buffer +  (f->row->sx - f->name_len - s->text_divider_len) + f->row->sx * (i - 1) + f->value_offset
				);
			}
		}
	}


/*
	for (int i = get_field_pos_line(f, pos, s); i < get_field_len_line(f, len, s); i++) {
		if (i == get_field_pos_line(f, pos, s)) {
			if (f->value_offset <= 0) {
				move(
					f->row->rx + get_field_pos_mod(f, pos, s),
					f->row->ry + get_field_pos_line(f, pos, s)
				);
				for (int j = 0; j < f->row->sx - get_field_pos_mod(f, pos, s); j++) {
					printf(" ");
				}
				move(
					f->row->rx + get_field_pos_mod(f, pos, s),
					f->row->ry + get_field_pos_line(f, pos, s)
				);

				printf(
					"%.*s",
					f->row->sx - get_field_pos_mod(f, pos, s),
					buffer + pos
					
				);
			} else {
				move(
					f->row->rx + f->name_len + s->text_divider_len,
					f->row->ry
				);
				for (int j = 0; j < f->row->sx - s->text_divider_len - f->name_len; j++) {
					printf(" ");
				}
				move(
					f->row->rx + f->name_len + s->text_divider_len,
					f->row->ry
				);

				printf(
					"%.*s",
					f->row->sx - s->text_divider_len - f->name_len,
					buffer + f->value_offset
				);
			}
		} else {
			move(f->row->rx, f->row->ry + i);
			for (int j = 0; j < f->row->sx; j++) {
				printf(" ");
			} 
			move(f->row->rx, f->row->ry + i);
			if (f->value_offset <= 0) {
				printf(
					"%.*s",
					f->row->sx,
					buffer + (f->row->sx - f->name_len - s->text_divider_len) + f->row->sx * (i - 1)
				);
			} else {
				printf(
					"%.*s",
					f->row->sx,
					buffer + (f->row->sx - f->name_len - s->text_divider_len) + f->row->sx * (i - 1) + f->value_offset
				);
			}
		}

	}
*/
	r.rc = 0;
	return r;
}

struct Result field_backspace(struct field *f, struct style *s)
{
	struct Result r;

	if (buf_pos > 0) {
		memmove(
			&buf[buf_pos - 1],
			&buf[buf_pos],
			buf_len - buf_pos + 1
		);

		buf_pos--;
		buf_len--;

		f->value_offset = get_field_len(f, buf_len, s) - (f->row->sx * f->max_entry_height);
		// Need extra condition here for if middle of string
		if (f->value_offset < 0) {
			printf("\b \b");
		}

		r = field_char_shift(f, buf, buf_pos, buf_len, (enum field_mode){ ENTRY }, s);
		field_cursor_shift(f, s);
	}

	r.rc = 0;
	return r;
}

struct Result field_char(struct field *f, struct style *s, int *c)
{
	struct Result r;

	// Only accept base ascii chars
	if (*c < 32 || *c > 126) {
		r.rc = 0;
		return r;
	}


	if (buf_len + 1 == f->max_entry_length) {
		r.rc = 0;
		return r;
	}


	// All of this logic has to be based on entry length, not line construct
	// to support limitless input, dot dot dots, etc.
	

	/* Not a fan of this code */

	// Need to do something here other than iterating buf_len
	// Either generalized function or something else
	/* */
	/*
	buf_len++;
	if (
		get_field_len_line(f, buf_len, s) > f->max_entry_height
		&& f->max_entry_height != 0
	) {
		buf_len--;
		r.rc = 0;
		return r;
	} else {
		buf_len--;
	}
	*/

	if (f->value_offset < 0) {
		if (get_field_len_line(f, buf_len + 1, s) > f->row->sy) {
			r = clear_horizontal_bar(
				f->column,
				f->row->ry + f->row->sy,
				s
			);

			f->row->sy++;
			r = draw_horizontal_bar(
				f->column,
				f->row->ry + f->row->sy,
				s
			);
		}
	}


	// Need to incorporate edge test here before iterating
	if (buf_pos < buf_len) {
		memmove(
			&buf[buf_pos + 1],
			&buf[buf_pos],
			buf_len - buf_pos
		);
	}

	buf[buf_pos] = *c;
	buf_len++;

	r = field_char_shift(f, buf, buf_pos, buf_len, (enum field_mode){ ENTRY }, s);
	buf_pos++;

	field_cursor_shift(f, s);

	r.rc = 0;
	return r;
}


struct Result field_switch(struct field *f, struct style *s)
{
	struct Result r;

	r = draw_field(f, s, (enum field_mode){ ENTRY });

	move(f->row->rx, f->row->ry);
	printf(f->name);

// Edge detection foo
/*
	int ypos;
	if (cc->cr > w->ws_row - 2) ypos = cc->ry + w->ws_row - 2;
	else ypos = cc->ry + cc->cr + 1;
*/
	move(f->row->rx + f->name_len + s->text_divider_len, f->row->ry);
	if (f->value != f->placeholder) {

		strcpy(buf, f->value);
		buf_pos = f->value_pos;
		buf_len = f->value_len;

		r = field_char_shift(f, buf, 0, (enum field_mode){ ENTRY}, buf_len, s);
		field_cursor_shift(f, s);
	} else {
		buf_len = 0;
		buf_pos = 0;
	}

//	Need to trigger menu redraw here to shift other rows down

	printf(CURSOR_SHOW);

	int c;
	while (buf_pos < MAX_BUF) {
		c = getchar();

		switch (c) {
			case ESCAPE:
				r = field_escape(f, s);
				break;
			case ENTER:
				r = field_enter(f, s);
				return r;
			case BACKSPACE:
				r = field_backspace(f, s);
				break;
			default:
				r = field_char(f, s, &c);
				break;
		}
		if (c == ENTER) break;
	}

	fflush(stdout);

	r.rc = 0;
	return r;
}


void move(int x, int y)
{
	printf("\033[%d;%dH", y, x);
}

void set_noncanonical_mode(int echo)
{
	struct termios term;

	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag &= ~(ICANON);
	if (echo) {
		term.c_lflag |= ECHO;
	} else {
		term.c_lflag &= ~ECHO;
	}

	term.c_cc[VMIN] = 1;
	term.c_cc[VTIME] = 0;

	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void handle_signal(int signal) {
	if (signal == SIGINT) {
		printf("\033[?25l");
		printf("\033[2J\033[H");
		printf("\033[?25h");
	}

	exit(0);
}

struct Result menu_switch(struct menu **m, struct winsize *w, struct style *s)
{
	struct Result r;

	int c;

	c = getchar();

	switch(c) {
		case RIGHT_ARROW:
			r = menu_r_arrow(m, w, s);
			break;
		case LEFT_ARROW:
			r = menu_l_arrow(m, w, s);
			break;
		case DOWN_ARROW:
			r = menu_d_arrow(*m, s);
			break;
		case UP_ARROW:
			r = menu_u_arrow(*m, s);
			break;
		case ENTER:
			r = menu_enter(m, w, s);
			break;
	}

	return r;
}

void clear_column(struct column *c)
{
	for (int i = 0; i < c->sy - 2; i++) {
		move(c->rx + 1, c->ry + 1 + i);
		for (int j = 0; j < c->sx - 2; j++)
		    printf(" ");
	}
}

bool edge_detect(struct column *c, int ry)
{
	for (int i = 0; i < c->nr; i++) {
		struct row *r = c->rs[i];
		switch (r->type) {
			case BREAK:
				if (r->ry > ry) {
					return false;
				} else if (r->ry == ry) {
					return true;
				} else {
					break;
				}
			case MENU:
				break;
			case FIELD:
				break;
		}
	}
	return false;
}

struct Result set_edge_vertical_bar_left(struct menu *m,
	struct style *s,
	const int tc,
	const int ry)
{
	struct Result r;

	if (m->cc == 0) {
		r.data = s->border->vertical;
	} else {
		if (edge_detect(m->cs[tc-1], ry)) {
			r.data = s->border->left_junction;
		} else {
			r.data = s->border->vertical;
		}
	}

	r.rc = 0;
	return r;
}

struct Result set_edge_vertical_bar(struct menu *m,
	struct style *s,
	const int tc,
	const int ry)
{
	struct Result r;
	r.rc = 0;
	r.msg = NULL;
	r.data = NULL;

	r = set_edge_vertical_bar_left(m, s, tc, ry);
	if (r.rc == 0) {
		move(m->cs[tc]->rx, ry);
		printf("%s", (char *)r.data);
	}

	r.rc = 0;
	return r;
}

struct Result set_edge_right_junction(struct menu *m,
	struct style *s,
	const int tc,
	const int ry)
{
	struct Result r;
	r.rc = 0;
	r.msg = NULL;
	r.data = NULL;

	if (tc == 0) {
		if (ry == 0) {
			r.data = s->border->top_left;
		} else {
			r.data = s->border->right_junction;
		}
	} else {
		if (ry == 1) {
			r.data = s->border->top_junction;
		} else {
			if (edge_detect(m->cs[tc-1], ry)) {
				r.data = s->border->center_junction;
			} else {
				r.data = s->border->right_junction;
			}
		}
	}

	r.rc = 0;
	return r;
}

struct Result set_edge_left_junction(struct menu *m,
				     struct style *s,
				     const int tc,
				     const int ry)
{
	struct Result r;
	r.rc = 0;
	r.msg = NULL;
	r.data = NULL;

	if (tc == m->nc - 1) {
		if (ry == 0) {
			r.data = s->border->top_right;
		} else {
			r.data = s->border->left_junction;
		}
	} else {
		if (ry == 1) {
			r.data = s->border->top_junction;
		} else {
			if (edge_detect(m->cs[tc+1], ry)) {
				r.data = s->border->center_junction;
			} else {
				r.data = s->border->left_junction;
			}
		}
	}

	r.rc = 0;
	return r;
}

struct Result set_edge(struct menu *m,
		       struct style *s,
		       const int tc,
		       const int ry,
		       const char *c)
{
	struct Result r;
	r.rc = 0;
	r.data = NULL;
	r.msg = NULL;

	uint32_t c_int = utf8_decode(c);
	if (c_int == utf8_decode(s->border->right_junction)) {
		r = set_edge_right_junction(m, s, tc, ry);
	} else if (c_int == utf8_decode(s->border->vertical)) {
		r = set_edge_vertical_bar(m, s, tc, ry);
	} else if (c_int == utf8_decode(s->border->left_junction)) {
		r = set_edge_left_junction(m, s, tc, ry);
	}
	
	return r;
}

struct Result utf8_encode(uint32_t c, char *o)
{
	struct Result r;

	if (c <= 0x7F) {
		o[0] = c;
		o[1] = '\0';
	
		r.rc = 1;
		return r;
	} else if (c <= 0x7FF) {
		o[0] = 0xC0 | (c >> 6);
		o[1] = 0x80 | (c & 0x3F);
		o[2] = '\0';
		
		r.rc = 2;
		return r;
	} else if (c <= 0xFFFF) {
		o[0] = 0xE0 | (c >> 12);
		o[1] = 0x80 | ((c >> 6) & 0x3F);
		o[2] = 0x80 | (c & 0x3F);
		o[3] = '\0';

		r.rc = 3;
		return r;
	} else if (c <= 0x10FFFF) {
		o[0] = 0xF0 | (c >> 18);
		o[1] = 0x80 | ((c >> 12) & 0x3F);
		o[2] = 0x80 | ((c >> 6) & 0x3F);
		o[3] = 0x80 | (c & 0x3F);
		o[4] = '\0';
		
		r.rc = 4;
		return r;
	}

	r.rc = 0;
	return r;
}

uint32_t utf8_decode(const char *c)
{
	uint32_t o;

	char c0 = (char)c[0];

	if (c0 <= 0x7F) {
		o = c0;
	} else if ((c0 & 0xE0) == 0xC0) {
		char c1 = (char)c[1];
		o = ((c0 & 0x1F) << 6)
			| (c1 & 0x3F);
	} else if ((c0 & 0xF0) == 0xE0) {
		char c1 = (char)c[1];
		char c2 = (char)c[2];
		o = ((c0 & 0x0F) << 12)
			| ((c1 & 0x3F) << 6)
			| (c2 & 0x3F);
	} else if ((c0 & 0xF8) == 0xF0) {
		char c1 = (char)c[1];
		char c2 = (char)c[2];
		char c3 = (char)c[3];
		o = ((c0 & 0x07) << 18)
			| ((c1 & 0x3F) << 12)
			| ((c2 & 0x3F) << 6)
			| (c3 & 0x3F);
	}

	return o;
}
