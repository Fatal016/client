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

char buf[MAX_BUF];

int main(int argc, char** argv)
{
	struct Result r;
	struct winsize w;
	struct menu_t *menu = &search_menu;

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
//	wprintf(CLEAR_DISPLAY);
	r = draw_module(menu, &w);
	r = set_style(menu, &w);

	while(1) {
		r = menu_switch(&menu, &w);
	}

	return r.rc;
}

struct Result draw_next_menu(struct menu_t **m, struct winsize *w)
{
	struct Result r;
	struct column_t *cc = (*m)->cs[(*m)->cc];

	wprintf(CLEAR_DISPLAY);
	*m = (struct menu_t *)(cc->rs[cc->cr]->data);
	r = draw_module(*m, w);
	set_style(*m, w);

	return r;
}

struct Result menu_r_arrow(struct menu_t **m, struct winsize *w)
{
	struct Result r;
	struct column_t *cc = (*m)->cs[(*m)->cc];

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

	return r;
}

struct Result menu_l_arrow(struct menu_t **m, struct winsize *w)
{
	struct Result r;
//	struct column_t *cc = (*m)->cs[(*m)->cc];

	if ((*m)->pm != NULL) {
		wprintf(CLEAR_DISPLAY);
		*m = (*m)->pm;
		r = draw_module(*m, w);
		r = set_style(*m, w);
	}

	r.rc = 0;
	return r;
}

struct Result menu_enter(struct menu_t **m, struct winsize *w)
{
	struct Result r;
	struct column_t *cc = (*m)->cs[(*m)->cc];
	struct row_t *cr = cc->rs[cc->cr];

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

	r.rc = 0;
	return r;
}


struct Result prompt_enter(struct menu_t *m, struct winsize *w)
{
	struct Result r;
	struct column_t *cc = m->cs[m->cc];
	struct prompt_t *p = (struct prompt_t *)cc->rs[cc->cr]->data;

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


	r.rc = 0;
	return r;
}

struct Result prompt_backspace(struct menu_t *m, struct winsize *w)
{
	struct Result r;
	struct column_t *cc = m->cs[m->cc];
	struct prompt_t *p = (struct prompt_t *)cc->rs[cc->cr]->data;

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

	r.rc = 0;
	return r;
}

struct Result prompt_escape(struct menu_t *m, struct winsize *w)
{
	struct Result r;
	struct column_t *cc = m->cs[m->cc];
	struct prompt_t *p = (struct prompt_t *)cc->rs[cc->cr]->data;

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

	r.rc = 0;
	return r;
}

struct Result prompt_char(struct menu_t *m, struct winsize *w, int *c)
{
	struct Result r;
	struct column_t *cc = m->cs[m->cc];
	struct prompt_t *p = (struct prompt_t *)cc->rs[cc->cr]->data;

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

	r.rc = 0;
	return r;
}






struct Result init_prompt(struct menu_t *m, struct winsize *w)
{
	struct Result r;
	struct column_t *cc = m->cs[m->cc];
	struct prompt_t *p = (struct prompt_t *)cc->rs[cc->cr]->data;

	draw_box_aware(m, cc->sx, p->height, cc->rx, cc->ry + cc->cr);

	int ypos;
	if (cc->cr > w->ws_row - 2) ypos = cc->ry + w->ws_row - 2;
	else ypos = cc->ry + cc->cr + 1;

	int offset = ((struct prompt_t *)(cc->rs[cc->cr]->data))->name_len + 1;
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

	r.rc = 0;
	return r;
}


void moveCursor(int x, int y)
{
	wprintf(L"\033[%d;%dH", y, x);
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

struct Result menu_switch(struct menu_t **m, struct winsize *w)
{
	struct Result r;
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
	return r;
}

struct Result prompt_switch(struct menu_t *m, struct winsize *w)
{
	struct Result r;
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

	r.rc = 0;
	return r;
}

void clear_column(struct menu_t *m)
{
	struct column_t *cc = m->cs[m->cc];

	for (int i = 0; i < cc->sy - 2; i++) {
		wprintf(L"\033[%d;%dH", cc->ry + 1 + i, cc->rx + 1);
		for (int j = 0; j < cc->sx - 2; j++)
		    wprintf(L" ");
	}
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

}
*/


struct Result set_edge_right_junction(struct menu_t *m, int ry, int c)
{
	struct Result r;
	r.rc = 0;
	r.msg = NULL;
	r.data = NULL;

	struct column_t *pc = m->cs[m->cc-1];
	
	for (int i = 0; i < pc->nr; i++) {
		struct row_t *pcr = pc->rs[i];
		switch (pcr->type) {
			case BREAK:
				if (pcr->ry == ry) {
					r.data = malloc(sizeof(wchar_t));
					*(wchar_t*)r.data = T_JUNCTION;
				}
				break;
			default:
				break;
		}
	}

	r.rc = 0;
	return r;
}

struct Result set_edge_vertical_bar(struct menu_t *m, int ry, int c)
{
	struct Result r;
	r.rc = 0;
	r.msg = NULL;
	r.data = NULL;

	struct column_t *pc = m->cs[m->cc-1];
	struct column_t *nc = m->cs[m->cc+1];

	for (int i = 0; i < pc->nr; i++) {
		struct row_t *pcr = pc->rs[i];
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

	r.rc = 0;
	return r;
}

struct Result set_edge(struct menu_t *m, int ry, int c)
{
	struct Result r;
	r.rc = 0;
	r.data = NULL;
	r.msg = NULL;

	switch (c) {
		case RIGHT_JUNCTION:
			r = set_edge_right_junction(m, ry, c);
			break;
		case VERTICAL_BAR:
			r = set_edge_vertical_bar(m, ry, c);
			break;
	}

	if (r.data == NULL) {
		r.data = (int*)malloc(sizeof(int));
		*(int*)r.data = c;
	}

	return r;
	/*
		case VERTICAL_BAR:
			r = set_edge_vertical_bar(m, rx, ry);
			break;
	*/}

struct Result utf8_encode(uint32_t *cp, char out[5])
{
	struct Result r;

	if (*cp <= 0x7F) {
		out[0] = *cp;
		out[1] = 0;
	
		r.rc = 1;
		return r;
	} else if (*cp <= 0x7FF) {
		out[0] = 0xC0 | (*cp >> 6);
		out[1] = 0x80 | (*cp & 0x3F);
		out[2] = 0;
		
		r.rc = 2;
		return r;
	} else if (*cp <= 0xFFFF) {
		out[0] = 0xE0 | (*cp >> 12);
		out[1] = 0x80 | ((*cp >> 6) & 0x3F);
		out[2] = 0x80 | (*cp & 0x3F);
		out[3] = 0;

		r.rc = 3;
		return r;
	} else if (*cp <= 0x10FFFF) {
		out[0] = 0xF0 | (*cp >> 18);
		out[1] = 0x80 | ((*cp >> 12) & 0x3F);
		out[2] = 0x80 | ((*cp >> 6) & 0x3F);
		out[3] = 0x80 | (*cp & 0x3F);
		out[4] = 0;
		
		r.rc = 4;
		return r;
	}

	r.rc = 0;
	return r;
}
/*
void print_utf8(uint32_t cp) {
    if (cp <= 0x7F) {
        putchar(cp);
    } else if (cp <= 0x7FF) {
        putchar(0xC0 | (cp >> 6));
        putchar(0x80 | (cp & 0x3F));
    } else if (cp <= 0xFFFF) {
        putchar(0xE0 | (cp >> 12));
        putchar(0x80 | ((cp >> 6) & 0x3F));
        putchar(0x80 | (cp & 0x3F));
    } else if (cp <= 0x10FFFF) {
        putchar(0xF0 | (cp >> 18));
        putchar(0x80 | ((cp >> 12) & 0x3F));
        putchar(0x80 | ((cp >> 6) & 0x3F));
        putchar(0x80 | (cp & 0x3F));
    }
}
*/
