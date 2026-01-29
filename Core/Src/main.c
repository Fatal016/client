#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <string.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <stdint.h>

#include "../Inc/tui.h"
#include "../Inc/menu.h"
#include "../Inc/templates.h"
#include "../Inc/style.h"

char buf[MAX_BUF];

int main(int argc, char** argv)
{
	struct Result r;
	struct winsize w;
	struct menu *m = &search_menu;

	struct style *s = (struct style *)malloc(sizeof(struct style));
	s->border = &BORDER_SINGLE;
	s->text = &BLACK_AND_WHITE;

	/* Initialization */
	setlocale(LC_CTYPE, "");
	signal(SIGINT, handle_signal);
	set_noncanonical_mode(0);
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
//	printf(CURSOR_STYLE);
//	printf(CURSOR_HIDE);
	
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
			break;
		case PROMPT:
//			struct prompt_t *p = (struct prompt_t *)cr->data;
			r = prompt_style(*m, w, s, ENTRY);
			break;
		default:
			break;
	}

	r.rc = 0;
	return r;
}


struct Result prompt_enter(struct menu *m, struct winsize *w, struct style *s)
{
	struct Result r;

	struct column *cc = m->cs[m->cc];
	struct prompt *p = (struct prompt *)cc->rs[cc->cr]->data;

	buf[strlen(buf)] = '\0';

	if (p->buf_pos > 0) {

		p->value = (char *)malloc((strlen(buf) + 1) * sizeof(char));
		strncpy(p->value, buf, strlen(buf) + 1);

		p->value_len = strlen(buf);
	} else {
		if (p->value != p->placeholder) {
			free(p->value);
			p->value = NULL;
		}
	}
/*
	r = set_edge(
		m,
		s,
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

struct Result prompt_backspace(struct menu *m, struct winsize *w, struct style *s)
{
	struct Result r;

	struct column *cc = m->cs[m->cc];
	struct prompt *p = (struct prompt *)cc->rs[cc->cr]->data;

	if (p->buf_pos > 0) {
		memmove(
			&buf[p->buf_pos - 1],
			&buf[p->buf_pos],
			strlen(buf) - p->buf_pos + 1
		);
		p->buf_pos--;
		printf("\033[D \033[D");
	}

	r.rc = 0;
	return r;
}

struct Result prompt_escape(struct menu *m, struct winsize *w, struct style *s)
{
	struct Result r;
/*
	struct column *cc = m->cs[m->cc];
	struct prompt *p = (struct prompt *)cc->rs[cc->cr]->data;

	int c;
	
	c = getchar();
	// '[' -> ANSI/VT100
	// 'O' -> Application Mode
	if (c == '[' || c == 'O') {
		c = getchar();
		if (c == 'C') {
			if (p->buf_pos < strlen(buf)) {
				p->buf_pos++;
				wprintf(L"\033[1C");
			}
		} else if (c == 'D') {
			if (p->buf_pos > 0) {
				p->buf_pos--;

				wprintf(L"\b");
			}

		}
	}
*/
	r.rc = 0;
	return r;
}

struct Result prompt_char(struct menu *m, struct winsize *w, int *c)
{
	struct Result r;

	struct column *cc = m->cs[m->cc];

	struct prompt *p = (struct prompt *)cc->rs[cc->cr]->data;

	if (*c >= 32 && *c <= 126) {
		if (p->buf_pos < strlen(buf)) {
			memmove(
				&buf[p->buf_pos + 1],
				&buf[p->buf_pos],
				strlen(buf) - p->buf_pos + 1
			);
			printf("\033[@");
		}

		buf[p->buf_pos] = *c;
		p->buf_pos++;

		if ((p->buf_pos % (cc->sx - p->name_len - 4)) == 0) {
			printf("\n\033[%dG", cc->rx + 2);
		}

		printf("%c", *c);
	}

	r.rc = 0;
	return r;
}


struct Result init_prompt(struct menu *m, struct winsize *w, struct style *s)
{
	struct Result r;

	struct column *cc = m->cs[m->cc];
	struct prompt *p = (struct prompt *)cc->rs[cc->cr]->data;

	draw_box_aware(m, s, cc->sx, p->height, cc->rx, cc->ry + cc->cr);

	int ypos;
	if (cc->cr > w->ws_row - 2) ypos = cc->ry + w->ws_row - 2;
	else ypos = cc->ry + cc->cr + 1;

	int offset = ((struct prompt *)(cc->rs[cc->cr]->data))->name_len + 1;
	move(cc->rx + 2 + offset, ypos);

	if (p->value == p->placeholder) {
		printf("\033[0m%*s\033[0m\n", p->value_len, " ");
		move(cc->rx + 2 + offset, ypos);
	} else {
		move(cc->rx + 2 + offset + p->value_len, ypos);
		p->buf_pos = p->value_len;
	}
	printf(CURSOR_SHOW);

	int c;

	while (p->buf_pos < MAX_BUF) {
		c = getchar();

		switch (c) {
			case ESCAPE:
//				r = prompt_escape(m, w);
				break;
			case ENTER:
				r = prompt_enter(m, w, s);
				return r;
			case BACKSPACE:
				r = prompt_backspace(m, w, s);
				break;
			default:
				r = prompt_char(m , w, &c);
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

struct Result prompt_switch(struct menu *m, struct winsize *w, struct style *s)
{
	struct Result r;
/*
	int c;

	c = getchar();

	switch(c) {
		case ESCAPE:
			break;
		case ENTER:
			break;
		default:
			break;
	}
*/
	r.rc = 0;
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
}

struct Result set_edge_vertical_bar_left(struct menu *m,
					 struct style *s,
					 const int *h,
					 struct edges *e)
{
	struct Result r;
	struct column *cc = m->cs[m->cc];
	struct row *cr = cc->rs[cc->cr];

	if (m->cc == 0) {
		e->top_left = s->border->vertical;
		e->bottom_left = s->border->vertical;
	} else {
		if (edge_detect(m->cs[m->cc-1], cr->ry - 1)) {
			e->top_left = s->border->left_junction;
		} else {
			e->top_left = s->border->vertical;
		}
	}

	return r;
}

struct Result set_edge_vertical_bar(struct menu *m,
				    struct style *s,
				    const int *h,
				    struct edges *e)
{
	struct Result r;
	r.rc = 0;
	r.msg = NULL;
	r.data = NULL;

	r = set_edge_vertical_bar_left(m, s, h, e);


	r.rc = 0;
	return r;
}

struct Result set_edge_right_junction(struct menu *m,
				      struct style *s,
				      const int ry)
{
	struct Result r;
	r.rc = 0;
	r.msg = NULL;
	r.data = NULL;

	struct column *cc = m->cs[m->cc];
	struct row *cr = cc->rs[cc->cr];

	if (m->cc == 0) {
	

		if (cr->ry == 0) {
			e->top_left = s->border->top_left;
		} else {
			e->top_left = s->border->right_junction;
		}
		e->bottom_left = s->border->right_junction;
	} else {
		if (cr->ry - 1 == 1) {
			e->top_left = s->border->top_junction;
		} else {
			if (edge_detect(m->cs[m->cc-1], cr->ry - 1)) {
				e->top_left = s->border->center_junction;
			} else {
				e->top_left = s->border->right_junction;
			}
		}
		if (edge_detect(m->cs[m->cc-1], cr->ry + *h)) {
			e->bottom_left = s->border->center_junction;
		} else {
			e->bottom_left = s->border->right_junction;
		}
	}

	r.rc = 0;
	return r;
}

struct Result set_edge_left_junction(struct menu *m,
				     struct style *s,
				     const int *h,
				     struct edges *e)
{
	struct Result r;
	r.rc = 0;
	r.msg = NULL;
	r.data = NULL;

	struct column *cc = m->cs[m->cc];
	struct row *cr = cc->rs[cc->cr];

	if (m->cc == m->nc - 1) {
		if (cr->ry == 0) {
			e->top_right = s->border->top_right;
		} else {
			e->top_right = s->border->left_junction;
		}
		e->bottom_right = s->border->left_junction;
	} else {
		if (cr->ry - 1 == 1) {
			e->top_right = s->border->top_junction;
		} else {
			if (edge_detect(m->cs[m->cc+1], cr->ry - 1)) {
				e->top_right = s->border->center_junction;
			} else {
				e->top_right = s->border->left_junction;
			}
		}
		if (edge_detect(m->cs[m->cc+1], cr->ry + *h)) {
			e->bottom_right = s->border->center_junction;
		} else {
			e->bottom_right = s->border->left_junction;
		}
	}

	r.rc = 0;
	return r;
}

const int resolve_height(struct menu *m)
{
	struct column *cc = m->cs[m->cc];
	struct row *cr = cc->rs[cc->cr];

	if (cr->type == PROMPT) {
		struct prompt *p = (struct prompt *)cr->data;
		if (p->mode == ENTRY) {
			return p->height + 2;
		}
	}

	return cr->height;
}

struct Result set_edge(struct menu *m,
		       struct style *s,
		       const int ry,
		       const char *c)
{
	struct Result r;
	r.rc = 0;
	r.data = NULL;
	r.msg = NULL;
/*
	struct edges e;
	e.top_left = 0;
	e.bottom_left = 0;
	e.top_right = 0;
	e.bottom_right = 0;
*/
	struct column *cc = m->cs[m->cc];
	struct row *cr = cc->rs[cc->cr];

	uint32_t c_int = utf8_decode(c);
	if (c_int == utf8_decode(s->border->right_junction)) {
		r = set_edge_right_junction(m, s, ry);
	} else if (c_int == utf8_decode(s->border->vertical)) {
//		r = set_edge_vertical_bar(m, s, ry);
	} else if (c_int == utf8_decode(s->border->left_junction)) {
//		r = set_edge_left_junction(m, s, ry);
	}
/*
	if (e.top_left) {
		move(cr->rx-2, cr->ry-1);
		printf("%s", e.top_left);
	}
	if (e.bottom_left) {
		move(cr->rx-2, cr->ry+ *h);
		printf("%s", e.bottom_left);
	}
	if (e.top_right) {
		move(cr->rx+cc->sx-3, cr->ry-1);
		printf("%s", e.top_right);
	}
	if (e.bottom_right) {
		move(cr->rx+cc->rx-3, cr->ry+ *h);
		printf("%s", e.bottom_right);
	}
*/
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
