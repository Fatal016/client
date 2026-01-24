#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <string.h>
#include <wchar.h>
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
	while(1) {}
/*
	while(1) {
		r = menu_switch(&m, &w);
	}
*/
	return r.rc;
}

struct Result draw_next_menu(struct menu **m, struct winsize *w)
{
	struct Result r;
/*
	struct column *cc = (*m)->cs[(*m)->cc];

	wprintf(CLEAR_DISPLAY);
	*m = (struct menu *)(cc->rs[cc->cr]->data);
	r = draw_module(*m, w);
	set_style(*m, w);
*/
	return r;

}

struct Result menu_r_arrow(struct menu **m, const struct winsize *w, const struct style *s)
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

struct Result menu_l_arrow(struct menu **m, const struct winsize *w, const struct style *s)
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

struct Result menu_enter(struct menu **m, const struct winsize *w, const struct style *s)
{
	struct Result r;
/*
	struct column *cc = (*m)->cs[(*m)->cc];
	struct row *cr = cc->rs[cc->cr];

	switch(cr->type) {
		case MENU:
			r = draw_next_menu(m, w);
			break;
		case FIELD:
			break;
		case PROMPT:
//			struct prompt_t *p = (struct prompt_t *)cr->data;
			r = prompt_style(*m, w, ENTRY);
			break;
		default:
			break;
	}
*/
	r.rc = 0;
	return r;
}


struct Result prompt_enter(struct menu *m, const struct winsize *w, const struct style *s)
{
	struct Result r;
/*
	struct column *cc = m->cs[m->cc];
	struct prompt *p = (struct prompt_t *)cc->rs[cc->cr]->data;

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

	r = set_edge(
		m,
		cc->rs[cc->cr]->ry - 1,
		VERTICAL_BAR
	);
	wprintf(L"\033[%d;%dH", cc->rs[cc->cr]->ry - 1, cc->rx);
	//wprintf(L"%lc", *(wchar_t*)r.data);
	//printf("%s", (char *)r.data);
//	print_utf8(*(uint32_t*)r.data);


	r = set_edge(
		m,
		cc->rs[cc->cr]->ry + ((struct prompt_t*)cc->rs[cc->cr]->data)->height,
		VERTICAL_BAR
	);
//	wprintf(L"\033[%d;%dH", cc->rs[cc->cr]->ry + ((struct prompt_t*)cc->rs[cc->cr]->data)->height, cc->rx);
//	wprintf(L"%lc", (wchar_t)r.data);

	fflush(stdout);;

*/
	r.rc = 0;
	return r;
}

struct Result prompt_backspace(struct menu *m, const struct winsize *w, const struct style *s)
{
	struct Result r;
/*
	struct column *cc = m->cs[m->cc];
	struct prompt *p = (struct prompt *)cc->rs[cc->cr]->data;

	if (p->buf_pos > 0) {
		memmove(
			&buf[p->buf_pos - 1],
			&buf[p->buf_pos],
			strlen(buf) - p->buf_pos + 1
		);
		p->buf_pos--;

		wprintf(L"\033[D");
		wprintf(L"\033[P");
	}
*/
	r.rc = 0;
	return r;
}

struct Result prompt_escape(struct menu *m, const struct winsize *w, const struct style *s)
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
/*
	struct column *cc = m->cs[m->cc];
	struct prompt *p = (struct prompt *)cc->rs[cc->cr]->data;

	if (*c >= 32 && *c <= 126) {
		if (p->buf_pos < strlen(buf)) {
			memmove(
				&buf[p->buf_pos + 1],
				&buf[p->buf_pos],
				strlen(buf) - p->buf_pos + 1
			);
			wprintf(L"\033[@");
		}

		buf[p->buf_pos] = *c;
		p->buf_pos++;

		wprintf(L"%c", *c);
	}
*/
	r.rc = 0;
	return r;
}






struct Result init_prompt(struct menu *m, struct winsize *w, struct style *s)
{
	struct Result r;

	struct column *cc = m->cs[m->cc];
	struct prompt *p = (struct prompt *)cc->rs[cc->cr]->data;

	draw_box_aware(m, s, cc->sx, p->height, cc->rx, cc->ry + cc->cr);
/*
	int ypos;
	if (cc->cr > w->ws_row - 2) ypos = cc->ry + w->ws_row - 2;
	else ypos = cc->ry + cc->cr + 1;

	int offset = ((struct prompt *)(cc->rs[cc->cr]->data))->name_len + 1;
	moveCursor(cc->rx + 2 + offset, ypos);

	if (p->value == p->placeholder) {
		wprintf(L"\033[0m%*s\033[0m\n", p->value_len, L" ");
		moveCursor(cc->rx + 2 + offset, ypos);
	} else {
		moveCursor(cc->rx + 2 + offset + p->value_len, ypos);
		p->buf_pos = p->value_len;
	}
	printf(CURSOR_SHOW);




	int c;

	while (p->buf_pos < MAX_BUF) {
		c = getchar();

		switch (c) {
			case ESCAPE:
				r = prompt_escape(m, w);
				break;
			case ENTER:
				r = prompt_enter(m, w);
				return r;
			case BACKSPACE:
				r = prompt_backspace(m, w);
				break;
			default:
				r = prompt_char(m , w, &c);
				break;
		}
		if (c == ENTER) break;
	}
*/
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
		wprintf(L"\033[?25l");
		wprintf(L"\033[2J\033[H");
		wprintf(L"\033[?25h");
	}

	exit(0);
}

struct Result menu_switch(struct menu **m, const struct winsize *w, const struct style *s)
{
	struct Result r;
/*
	int c;

	c = getchar();

	switch(c) {
		case RIGHT_ARROW:
			r = menu_r_arrow(m, w);
			break;
		case LEFT_ARROW:
			r = menu_l_arrow(m, w);
			break;
		case ENTER:
			r = menu_enter(m, w);
			break;
	}
*/
	return r;
}

struct Result prompt_switch(struct menu *m, const struct winsize *w, const struct style *s)
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
/*
	struct column *cc = m->cs[m->cc];

	for (int i = 0; i < cc->sy - 2; i++) {
		wprintf(L"\033[%d;%dH", cc->ry + 1 + i, cc->rx + 1);
		for (int j = 0; j < cc->sx - 2; j++)
		    wprintf(L" ");
	}
*/
}

/*
struct Result set_edge_vertical_bar(struct menu_t *m, int *rx, int *ry)
{

	struct column_t *cc = m->cs[m->cc];

	if (m->cc > 0) {
		struct column_t *pc = m->cs[m->cc-1];
		for (int i = 0; i < pc->nr) {
			switch (pc->rs[i]->type) {
				case MENU:
					break;
				case FIELD:
					break;

				case PROMPT:
				
					break;
			}
		}
	}

	if (m->cc < m->nc - 1) {
		
	}


	struct Result r;
	return r;
}
*/


struct Result set_edge_right_junction(struct menu *m, struct style *s, int ry)
{
	struct Result r;
	r.rc = 0;
	r.msg = NULL;
	r.data = NULL;

	struct column *pc = m->cs[m->cc-1];
	
	for (int i = 0; i < pc->nr; i++) {
		struct row *pcr = pc->rs[i];
		switch (pcr->type) {
			case BREAK:
				if (pcr->ry == ry) {
				/*
					r.data = malloc(5 * sizeof(char));
					*(char *)r.data = *s->border->center_junction;
				*/
					r.data = (void *)s->border->center_junction;
				}
				break;
			default:
				break;
		}
	}

	r.rc = 0;
	return r;
}

struct Result set_edge_vertical_bar(struct menu *m, struct style *s, int *ry)
{
	struct Result r;
	r.rc = 0;
	r.msg = NULL;
	r.data = NULL;
/*
	struct column *pc = m->cs[m->cc-1];
	struct column *nc = m->cs[m->cc+1];

	for (int i = 0; i < pc->nr; i++) {
		struct row *pcr = pc->rs[i];
		switch (pcr->type) {
			case BREAK:
				if (pcr->ry == ry) {
							
					r.data = (int*)malloc(sizeof(int));
					*(int*)r.data = LEFT_JUNCTION;
				}
				break;
			default:
				break;
		}
	}
	for (int i = 0; i < nc->nr; i++) {
	//	struct row_t *ncr = nc->rs[i];
	}
*/
	r.rc = 0;
	return r;
}

struct Result set_edge(struct menu *m, struct style *s, int ry, const char *c)
{
	struct Result r;
	r.rc = 0;
	r.data = NULL;
	r.msg = NULL;

	uint32_t c_int = utf8_decode(c);
	if (c_int == utf8_decode(s->border->right_junction)) {
		r = set_edge_right_junction(m, s, ry);
	} else if (c_int == utf8_decode(s->border->vertical)) {
		 //r = set_edge_vertical_bar(m, s, ry);
	}
/*
	if (r.data == NULL) {
		r.data = (int*)malloc(sizeof(int));
		*(int*)r.data = c;
	}
*/
	return r;
	/*
		case VERTICAL_BAR:
			r = set_edge_vertical_bar(m, rx, ry);
			break;
	*/}

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
