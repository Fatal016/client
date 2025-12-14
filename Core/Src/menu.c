#include <stdlib.h>
#include <wchar.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "../Inc/menu.h"
#include "../Inc/tui.h"

struct Result draw_menu(struct menu_t *m)
{
	struct Result r;

	m->len = strlen(m->pn);
	wprintf(L"%s\n", m->pn);

	r.rc = 0;

	r.data = (void *)malloc(sizeof(int));
	*(int *)(r.data) = m->len;

	return r;
}

struct Result draw_field(struct field_t *f)
{
	struct Result r;

	return r;
}

// Will probably want global buffer for user input

struct Result draw_prompt(struct prompt_t *p)
{
	struct Result r;

	p->name_len = strlen(p->name);
	p->value_len = 0;

	if (p->value == NULL) {
		p->value = p->placeholder;
	}
// Will probably want global buffer for user input
	wprintf(L"%s %s", p->name, p->value);
	p->value_len = strlen(p->value);

	p->len = p->name_len + p->value_len + 1;

	r.rc = 0;
	r.data = (void *)malloc(sizeof(int));
	*(int *)(r.data) = p->len;

	return r;
}

struct Result draw_row(struct row_t *row, struct winsize *w)
{
	struct Result r;

	switch(row->type) {
		case MENU:
			r = draw_menu((struct menu_t *)row->data);
			break;
		case FIELD:
			r = draw_field((struct field_t *)row->data);
			break;
		case PROMPT:
			r = draw_prompt((struct prompt_t *)row->data);
			break;
	}

	return r;
}

struct Result draw_column(struct column_t *c, struct winsize *w)
{
	struct Result r;
	int max_size = 0;

	for (int i = 0; i < c->sy; i++) {
		/* c->rx + 2 -> bump two from edge, one space off border */
		/* c->ry + 1 + i -> bump from edge, no space */
		moveCursor(c->rx + 2, c->ry + 1 + i);
		c->rs[i]->rx = c->rx + 2;
		c->rs[i]->ry = c->ry + 1 + i;
		r = draw_row(c->rs[i], w);
		if (r.rc != 0) return r;
		if (*(int *)r.data > max_size) {
			max_size = *(int *)r.data;
		}
	}

	r.rc = 0;
	return r;
}

struct Result draw_module(struct menu_t *m, struct winsize *w)
{
	struct Result r;

	for (int i = 0; i < m->nc; i++) {
		/* Column reference */
		/* Just doing 1, 1 for now since multi-column is not present */
		m->cs[i]->rx = 1;
		m->cs[i]->ry = 1;
		r = draw_column(m->cs[i], w);
		if (r.rc != 0) return r;
		if (*(int *)r.data > m->sx) {
			m->sx = *(int *)r.data;
		}
	}

	draw_box(m->sx, m->nc, m->rx, m->ry);

	// Iterate over each column
	// Set rx and ry index for each column and then exec draw col
/*
	for (int i = 0; i < m->sy; i++) {
		moveCursor(m->rx + 2, m->ry + 1 + i);
		wprintf(L"%s", ((struct prompt_t *)m->cs[0]->rs[i]->data)->name);
	}
*/
	r.rc = 0;
	return r;
}
/*
int draw_field(struct menu_t *menu) { struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	int size_x = w.ws_col - 2;
	int size_y = w.ws_row - 2;

	if (w.ws_col > menu->size_x) {
		size_x = menu->size_x;
	}

	if (w.ws_row > menu->size_y) {
		size_y = menu->size_y;
	}
	draw_box(size_x, size_y, menu->ref_x, menu->ref_y);
	
	for (int i = 0; i < size_y; i++) {
		moveCursor(menu->ref_x + 2, menu->ref_y + 1 + i);
		wprintf(L"%ls %ls", ((struct field_t**)menu->items)[i + menu->item_offset]->field_name, ((struct field_t**)menu->items)[i + menu->item_offset]->field_value);
	}
	return 0;
}
*/
/*
int clear_style(struct menu_t *menu, struct winsize *w) {
	
	if (menu->cur_y > w->ws_row - 2) {
		moveCursor(menu->ref_x + 1, menu->ref_y + w->ws_row - 2);
	} else {
		moveCursor(menu->ref_x + 1, menu->ref_y + menu->cur_y);
	}

	// Reset text highlighting
	if (menu->type == MENU) {
		wprintf(L"\033[0m%*s", menu->size_x + 1, "");
	} else if (menu->type == FIELD) {
		wprintf(L"\033[0m%*s", wcslen(((struct field_t**)menu->items)[menu->cur_y - 1]->field_name) + 1, "");
	}
	
	if (menu->cur_y > w->ws_row - 2) {
		moveCursor(menu->ref_x + 1, menu->ref_y + w->ws_row - 2);
	} else {
		moveCursor(menu->ref_x + 1, menu->ref_y + menu->cur_y);
	}

	// Redraw text with default text color
	if (menu->type == MENU) {
		wprintf(L"\033[0m %ls", ((struct menu_t**)menu->items)[menu->cur_y - 1]->pretty_name);
	} else if (menu->type == FIELD) {
		wprintf(L"\033[0m %ls", ((struct field_t**)menu->items)[menu->cur_y - 1]->field_name);
	}

	return 0;
}
*/

struct Result prompt_style(struct menu_t *m, struct winsize *w, enum prompt_mode mode)
{
	struct Result r;
	struct column_t *cc = m->cs[m->cc];
	struct prompt_t *p = (struct prompt_t *)cc->rs[cc->tr]->data;


	int ypos;
	if (cc->tr > w->ws_row - 2) ypos = cc->ry + w->ws_row - 2;
	else ypos = cc->ry + cc->tr + 1;

	switch (mode) {
		case TRAVERSE:
			p->mode = TRAVERSE;
			moveCursor(cc->rx + 2, ypos);
			wprintf(L"\033[30;47m%*s\033[0m\n", p->name_len, p->name);
			p->mode = TRAVERSE;
			break;
		case ENTRY:
			p->mode = ENTRY;
			moveCursor(cc->rx + 2, ypos);
			wprintf(L"\033[0m%*s\033[0m\n", p->name_len, p->name);
			r = init_prompt(m, w);

			prompt_style(m, w, TRAVERSE);
			//moveCursor(cc->rx + 1 + offset, ypos);
			//wprintf(L"\033[30;47m%*s\033[0m\n", p->name_len, p->name);
			break;
	}

	r.rc = 0;
	return r;
}
/*
struct Result menu_style(struct menu_t *m, struct winsize *w)
{

}
*/
struct Result set_style(struct menu_t *m, struct winsize *w)
{
	// Handles default style case for row type
	// for menu-switch, etc.
	// More granular styling managed in directional (arrows,etc.) switch cases

	struct Result r;
	struct column_t *cc = m->cs[m->cc];

	if (cc->tr > w->ws_row - 2) {
		moveCursor(cc->rx + 1, cc->ry + w->ws_row - 2);
	} else {
		moveCursor(cc->rx + 2, cc->ry + cc->tr + 1);
	}


	switch (cc->rs[cc->tr]->type) {
		case MENU:
			wprintf(L"\033[30;47m%*s\033[0m\n", m->sx, ((struct menu_t *)cc->rs[cc->tr]->data)->pn);
			break;
		case PROMPT:
			r = prompt_style(m, w, (enum prompt_mode){ TRAVERSE });
//			wprintf(L"\033[30;47m%*s\033[0m\n", )

	//		wprintf(L"\033[30;47m%*s\033[0m\n", m->sx, ((struct menu_t *)cc->rs[cc->tr]->data)->pn);
		case FIELD:
			break;
	}

/*
	if (cc->tr > w->ws_row - 2) {
		moveCursor(cc->rx + 1, cc->ry + w->ws_row - 2);
	} else {
		moveCursor(cc->rx + 1, cc->ry + cc->tr + 1);
	}
*/
/*
	if (menu->cur_y > w->ws_row - 2) {
		moveCursor(menu->ref_x + 1, menu->ref_y + w->ws_row - 2);
	} else {
		moveCursor(menu->ref_x + 1, menu->ref_y + menu->cur_y);
	}
*/
/*
	switch (cc->rs[cc->tr]->type) {
		case MENU:
			wprintf(L"\033[30m %s", ((struct menu_t *)cc->rs[cc->tr]->data)->pn);
			break;
	}
*/
/*
if (menu->type == MENU) {
		wprintf(L"\033[30m %ls", ((struct menu_t**)menu->items)[menu->cur_y - 1]->pretty_name);
	} else if (menu->type == FIELD) {
		wprintf(L"\033[30m %ls", ((struct field_t**)menu->items)[menu->cur_y - 1]->field_name);
	}
	return 0;
*/
	r.rc = 0;
	return r;
}

/* Can clear any horizontal space to blank space before instantiating box */
/* To avoid flickering */
int draw_box(int sx, int sy, int rx, int ry) 
{
	wprintf(L"\033[%d;%dH", ry, rx);

	/* Upper bar */
	wprintf(L"%lc", 0x250C);
	for (int i = 0; i < sx + 2; i++) {
		wprintf(L"%lc", 0x2500);
	}
	wprintf(L"%lc\n", 0x2510);

	/* Sides */
	for (int i = 0; i < sy; i++) {
		if (rx != 1) {
			wprintf(L"\033[%dC", rx - 1);
		}
		wprintf(L"%lc\033[%dC%lc\n", 0x2502, sx + 2, 0x2502);
	}	

	/* Lower Bar */
	if (rx != 1) {
		wprintf(L"\033[%dC", rx - 1);
	}
	wprintf(L"%lc", 0x2514);
	

	for (int i = 0; i < sx + 2; i++) {
		wprintf(L"%lc", 0x2500);
	}
	wprintf(L"%lc\n", 0x2518);

	return 0;
}

/*
int resize_menu(struct menu_t* m)
{
	size_t max = 0;
	for 

	m->sx = 1;


//	m->sx = max_size(m) + 1;
	return 0;
}
*/
/*
int max_size(struct menu_t* menu)
{
	size_t max = 0, len;

	for (int i = 0; i < menu->size_y; i++) {
		len = 0;

		if (menu->type == MENU) {
			len = wcslen(((struct menu_t**)menu->items)[i]->pretty_name);
		} else {
			len = wcslen(((struct field_t**)menu->items)[i]->field_name) + 1;
			if (((struct field_t**)menu->items)[i]->field_value != NULL) {
				len += wcslen(((struct field_t**)menu->items)[i]->field_value);
			} else {
				len += 6;
			}
		}
		if (len > max) {
			max = len;
		}
	}

	return (int)max;
}
*/
