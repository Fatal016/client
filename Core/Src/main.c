#include <stdio.h> // eliminate
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <string.h> // unsure
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <stdint.h>

#include "../Inc/tui.h"
#include "../Inc/menu.h"
#include "../Inc/templates.h"
#include "../Inc/style.h"

char buf[MAX_BUF];
size_t buf_pos;
size_t buf_len;

int main(int argc, char** argv)
{
	struct Result r;
	struct winsize w;
	struct menu *m = &search_menu;

	struct style *s = style_init(
				&BORDER_SINGLE,
				&BLACK_AND_WHITE,
				": ",
				3,
				4
	);

	/* Initialization */
	setlocale(LC_CTYPE, "");
	signal(SIGINT, handle_signal);
	set_noncanonical_mode(0);
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
//	printf(CURSOR_STYLE);
	printf(CURSOR_HIDE);
	
	// ANSI Mode?
//	printf("\033[?1l");

	printf(CLEAR_DISPLAY);
	r = draw_module(m, &w, s);
	r = set_style(m, &w, s);

	while(1) {
		r = menu_switch(&m, &w, s);
	}

	return r.rc;
}

struct Result draw_next_menu(struct menu **m, struct winsize *w, struct style *s)
{
	struct Result r;

	struct column *cc = (*m)->cs[(*m)->cc];

	printf(CLEAR_DISPLAY);
	*m = (struct menu *)(cc->rs[cc->cr]->data);
	r = draw_module(*m, w, s);
	set_style(*m, w, s);

	return r;

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
			field_style(*m, w, s, (enum field_mode){ ENTRY });
			break;
		default:
			break;
	}

	r.rc = 0;
	return r;
}


struct Result field_enter(struct menu *m, struct winsize *w, struct style *s)
{
	struct Result r;

	struct column *cc = m->cs[m->cc];
	struct field *f = (struct field *)cc->rs[cc->cr]->data;

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


struct Result field_escape(struct menu *m, struct winsize *w, struct style *s)
{
	struct Result r;

	struct column *cc = m->cs[m->cc];
	struct row *cr = cc->rs[cc->cr];

	struct field *f = (struct field *)cr->data;

	int c;
/*	
	c = getchar();
	if (c != 0x1b) {
		return r;
	}
*/
	c = getchar();
	// '[' -> ANSI/VT100
	// 'O' -> Application Mode
	if (c == '[' || c == 'O') {
		c = getchar();
		if (c == 'C') {
			if (buf_pos < buf_len) {
				buf_pos++;
				r = field_char_check_edge(m, s);
				if (r.rc) {
					move(cc->rx + s->border_padding_left, cr->ry + r.rc);
				}
				printf("\033[1C");
			}
		} else if (c == 'D') {
			if (buf_pos > 0) {
				r = field_char_check_edge(m, s);
				buf_pos--;
				if (r.rc) {
					move(cr->rx + cr->sx - 1, cr->ry + r.rc - 1);
				} else {
					printf("\b");
				}
			}

		}
	}

	r.rc = 0;
	return r;
}

struct Result field_char_check_edge(struct menu *m, struct style *s)
{
	struct Result r;

	struct column *cc = m->cs[m->cc];
	struct row *cr = cc->rs[cc->cr];

	struct field *f = (struct field *)cc->rs[cc->cr]->data;

	int num = (f->name_len + s->text_divider_len + buf_pos);
	int den = cr->sx;

	if (num % den == 0) {
		r.rc = num/den;
		return r;
	}

	r.rc = 0;
	return r;
}

int get_field_height(struct field *f, struct style *s, enum field_mode fm)
{
	int h = 0;

	switch (fm) {
		case ENTRY:
			if (f->min_entry_height > get_field_len(f, s)) {
				h = f->min_entry_height;
			} else {
				h = get_field_len_line(f, s);
			}
			break;
	}

	return h;
}

int get_field_pos(struct field *f, struct style *s)
{
	return f->name_len + s->text_divider_len + buf_pos;
}

int get_field_pos_line(struct field *f, struct style *s)
{
	return (f->name_len + s->text_divider_len + buf_pos)/f->row->sx;
}

int get_field_pos_mod(struct field *f, struct style *s)
{
	return (f->name_len + s->text_divider_len + buf_pos) % f->row->sx;
}

int get_field_len(struct field *f, struct style *s)
{
	return f->name_len + s->text_divider_len + buf_len;
}

int get_field_len_line(struct field *f, struct style *s)
{
	return (f->name_len + s->text_divider_len + buf_len)/f->row->sx + 1;
}

int get_field_len_mod(struct field *f, struct style *s)
{
	return (f->name_len + s->text_divider_len + buf_len) % f->row->sx;
}

struct Result field_char_shift(struct field *f, struct style *s)
{
	struct Result r;

	int pos = get_field_pos(f, s);
	int lref = pos/f->row->sx;

	int len = get_field_len(f, s);
	int llen = len/f->row->sx + 1;

	for (int i = get_field_pos_line(f, s); i < get_field_len_line(f, s); i++) {
		if (i == lref) {
			move(
				f->row->rx + get_field_pos_mod(f, s),
				f->row->ry + get_field_pos_line(f, s)
			);
			for (int j = 0; j < f->row->sx - get_field_pos_mod(f, s); j++) {
				printf(" ");
			}
			move(
				f->row->rx + get_field_pos_mod(f, s),
				f->row->ry + get_field_pos_line(f, s)
			);

			printf(
				"%.*s",
				f->row->sx - get_field_pos_mod(f, s),
				buf + buf_pos
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
				buf + (f->row->sx - f->name_len - s->text_divider_len) + f->row->sx * (i - 1));
		}
	}
	return r;
}

struct Result field_backspace(struct menu *m, struct winsize *w, struct style *s)
{
	struct Result r;

	struct column *cc = m->cs[m->cc];
	struct row *cr = cc->rs[cc->cr];

	struct field *f = (struct field *)cr->data;

	if (buf_pos > 0) {
		memmove(
			&buf[buf_pos - 1],
			&buf[buf_pos],
			buf_len - buf_pos + 1
		);

		buf_pos--;
		buf_len--;

		r = field_char_shift(f, s);
		
		move(
			cr->rx + get_field_pos_mod(f, s),
			cr->ry + get_field_pos_line(f, s)
		);
	}

	r.rc = 0;
	return r;
}

struct Result field_char(struct menu *m, struct winsize *w, struct style *s, int *c)
{
	struct Result r;

	struct column *cc = m->cs[m->cc];
	struct row *cr = cc->rs[cc->cr];

	struct field *f = (struct field *)cc->rs[cc->cr]->data;

	if (*c < 32 || *c > 126) {
		r.rc = 0;
		return r;
	}

	if (buf_pos < buf_len) {
		// increment buff len and have handler for resize, etc.
		memmove(
			&buf[buf_pos + 1],
			&buf[buf_pos],
			buf_len - buf_pos
		);
		buf[buf_pos] = *c;
		buf_len++;

		r = field_char_shift(f, s);
	} else {
		buf[buf_pos] = *c;
		buf_len++;

	} 

	int x = get_field_pos_mod(f, s);
	int y = get_field_pos_line(f, s);



	move(
		cr->rx + get_field_pos_mod(f, s),
		cr->ry + get_field_pos_line(f, s)
	);


	buf_pos++;
	printf("%c", *c);
	r = field_char_check_edge(m,s);

	if (r.rc) {
		move(cc->rx + s->border_padding_left + 1, cc->ry + r.rc + 1);
	}

/*
	if (get_field_pos_line(m, s) >= p->height) {
		r = clear_horizontal_bar(m, s, m->cc, cr->ry + get_field_len_line(m, s) - 1);
	}
*/
/*	
	if (p->cr == p->height) {
		r = clear_horizontal_bar(m, s, m->cc, cr->ry + p->cr-1);
		p->height += 5;

		printf("\n\033[%dG", cc->rx + 2);


		draw_box_aware(m, s);
	}
*/
	r.rc = 0;
	return r;
}


struct Result field_switch(struct menu *m, struct winsize *w, struct style *s)
{
	struct Result r;

	struct column *cc = m->cs[m->cc];
	struct row *cr = cc->rs[cc->cr];
	struct field *f = (struct field *)cr->data;

	r = draw_field(f, s, (enum field_mode){ ENTRY });


	move(cr->rx, cr->ry);

	// Is length param really necessary here?
	printf("\033[0m%*s\033[0m\n", f->name_len, f->name);

/*
	int ypos;
	if (cc->cr > w->ws_row - 2) ypos = cc->ry + w->ws_row - 2;
	else ypos = cc->ry + cc->cr + 1;
*/
	move(cr->rx + f->name_len + s->text_divider_len, cr->ry);
	if (f->value != f->placeholder) {
		buf_pos = f->value_pos;
		buf_len = f->value_len;
//		printf("%s", f->value);
	} else {
		buf_len = 0;
		buf_pos = 0;
//		p->value_len = 0;
	}


	printf(CURSOR_SHOW);

	int c;
	while (buf_pos < MAX_BUF) {
		c = getchar();

		switch (c) {
			case ESCAPE:
				r = field_escape(m, w, s);
				break;
			case ENTER:
				r = field_enter(m, w, s);
				return r;
			case BACKSPACE:
				r = field_backspace(m, w, s);
				break;
			default:
				r = field_char(m , w, s, &c);
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
		case ENTER:
			r = menu_enter(m, w, s);
			break;
	}

	return r;
}

void clear_column(struct menu *m)
{
	struct column *cc = m->cs[m->cc];

	for (int i = 0; i < cc->sy - 2; i++) {
		move(cc->rx + 1, cc->ry + 1 + i);
		for (int j = 0; j < cc->sx - 2; j++)
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
	struct column *cc = m->cs[tc];

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

	struct column *cc = m->cs[tc];
	struct row *cr = cc->rs[cc->cr];

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

	struct column *cc = m->cs[tc];
	struct row *cr = cc->rs[cc->cr];

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

int resolve_height(struct menu *m)
{
	struct column *cc = m->cs[m->cc];
	struct row *cr = cc->rs[cc->cr];
/*
	if (cr->type == FIELD) {
		struct field *f = (struct field *)cr->data;
		if (f->mode == ENTRY) {
			return f->height;
		}
	}

	return cr->height;
*/
	return 1;
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
