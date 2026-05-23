#include <stdlib.h>
#include <wchar.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "../Inc/menu.h"
#include "../Inc/tui.h"
#include "../Inc/style.h"

struct Result draw_menu(struct menu *m, struct style *s)
{
	struct Result r;

	m->len = strlen(m->pn);
	printf("%s\n", m->pn);

	r.rc = 0;

	r.data = (void *)malloc(sizeof(int));
	*(int *)(r.data) = m->len;

	return r;
}

// Can accept extra mode var here?
struct Result draw_field(struct field *f, struct style *s)
{
	struct Result r;
/*
	if (p->mode == NULL) {
		p->mode = TRAVERSE;
	}
*/
	if (f->value == NULL) {
		f->value = f->placeholder;
	}

	f->name_len = strlen(f->name);
	f->value_len = strlen(f->value);
//	p->len = p->name_len + s->text_divider_len + p->value_len;

//	p->pos = p->name_len + s->text_divider_len + p->value_pos;


	// Replace with generalized cascade
	printf("%s%s%s", f->name, s->text_divider, f->value);

	r.rc = 0;
	r.data = (void *)malloc(sizeof(int));

	// Convert to generalized get_field_len function
	// get_field_len currently relies on active context of row
	*(int *)(r.data) = f->name_len + s->text_divider_len + f->value_len;

	return r;
}

struct Result draw_row(struct menu *m,
		       struct winsize *w,
		       struct style *s,
		       int tc,
		       int tr)
{
	struct Result r;
	
	struct column *cc = m->cs[tc];
	struct row *cr = cc->rs[tr];

	move(cr->rx, cr->ry);

	switch(cr->type) {
		case MENU:
			r = draw_menu((struct menu *)cr->data, s);
			break;
		case FIELD:
			r = draw_field((struct field *)cr->data, s);
			break;
		case BREAK:
			r = draw_horizontal_bar(m, s, tc, cr->ry);
	}

	return r;
}

struct Result draw_column(struct menu *m,
			  struct winsize *w,
			  struct style *s,
			  int tc)
{
	struct Result r;
	int max_size = 0;

	struct column *cc = m->cs[tc];
	struct row *cr;

	for (int i = 0; i < cc->nr; i++) {

		cr = cc->rs[i];

		cr->rx = cc->rx + 1 + s->border_padding_left;
		cr->ry = cc->ry + 1 + i;

		cr->sx = cc->sx - (2 + s->border_padding);
		cr->sy = 1;

		r = draw_row(m, w, s, tc, i);
		if (r.rc != 0) return r;
		if (*(int *)r.data > max_size) {
			max_size = *(int *)r.data;
		}
	}

	r.rc = 0;
	return r;
}

struct Result init_module(struct menu *m, struct winsize *w)
{
	struct Result r;

	if (!m->cc_set) {
		m->cc = 0;
		m->cc_set = true;
	}

	if (!m->rx_set) {
		m->rx = 1;
		m->rx_set = true;
	}

	if (!m->ry_set) {
		m->ry = 1;
		m->ry_set = true;
	}

	if (!m->cp_set && m->nc > 1) {
		m->cp = SPLIT;
		m->cp_set = true;
	}
	

	r.rc = 0;
	return r;
}

struct Result scale_module(struct menu *m, struct winsize *w)
{
	struct Result r;

	m->sx = w->ws_col;
	m->sy = w->ws_row;

	switch(m->cp) {
		case SPLIT:
			for (int i = 0; i < m->nc; i++) {
				m->cs[i]->rx = 1 + i * (m->sx / m->nc);
				m->cs[i]->ry = 1;

				if (i == m->nc - 1) {
					m->cs[i]->sx = m->sx - m->cs[i]->rx + 1;
				} else {
					m->cs[i]->sx = 2 + ((i + 1) * (m->sx / m->nc)) - m->cs[i]->rx;
				}

				m->cs[i]->sy = m->sy;
			}
			break;
		default:
			break;
	}

	r.rc = 0;
	return r;
}


struct Result clear_horizontal_bar(struct menu *m,
				   struct style *s,
				   int tc,
				   int ry)
{
	struct Result r;

	move(m->cs[tc]->rx, ry);

//	r = set_edge(m, s, tc, ry, s->border->vertical);

/*
	if (r.rc == 0) {
		printf("%s", (char*)r.data);
	}
*/


	for (int i = m->cs[tc]->rx + 1; i < m->cs[tc]->rx + m->cs[tc]->sx; i++) {
		printf("%s", " ");
	}
/*
	r = set_edge(m, s, tc, ry, s->border-vertical);
	if (r.rc == 0) {
		printf("%s", (char *)r.data);
	}
*/
	r.rc = 0;
	return r;
}

struct Result draw_horizontal_bar(struct menu *m,
				  struct style *s,
				  int tc,
				  int ry)
{
	struct Result r;

	struct column *cc = m->cs[m->cc];

	move(m->cs[tc]->rx, ry);

	r = set_edge(m, s, tc, ry, s->border->right_junction);
	if (r.rc == 0) {
		printf("%s", (char*)r.data);
	}

	for (int i = m->cs[tc]->rx + 1; i < m->cs[tc]->rx + m->cs[tc]->sx - 1; i++) {
		printf("%s", s->border->horizontal);
	}

	r = set_edge(m, s, tc, ry, s->border->left_junction);
	if (r.rc == 0) {
		printf("%s", (char*)r.data);
	}


	r.rc = 0;
	return r;
}


struct Result draw_vertical_bar(int ys, int ye, int rx, struct style *s)
{
	struct Result r;

	printf("\033[%d;%dH%s", ys, rx, s->border->top_junction);
	for (int i = ys + 1; i < ye + 1 - 1; i++) {
		printf("\033[%d;%dH%s", i, rx, s->border->vertical);
	}
	printf("\033[%d;%dH%s", ye, rx, s->border->bottom_junction);

	r.rc = 0;
	return r;
}

struct Result draw_column_dividers(struct menu *m, struct style *s)
{
	struct Result r;

	r.rc = 0;
	if (m->nc < 2) return r;

	for (int i = 1; i < m->nc; i++) {
		switch(m->cp) {
			case SPLIT:
				r = draw_vertical_bar(
					m->cs[i]->ry,
					m->sy,
					m->cs[i]->rx,
					s
				);
				break;
			default:
				break;
		}
	}

	return r;
}

struct Result draw_dividers(struct menu *m, struct style *s)
{
	struct Result r;

	r = draw_column_dividers(m, s);

	r.rc = 0;
	return r;
}


struct Result draw_module(struct menu *m, struct winsize *w, struct style *s)
{
	struct Result r;

	init_module(m, w);
	scale_module(m, w);
	draw_box_general(m->sx, m->sy, m->rx, m->ry, s);
	r = draw_dividers(m, s);
/*
	if (!m->pending_init) {
		r = init_module(m, w);
		m->pending_init = true;
	}

	if (!m->pending_scale) {
		r = scale_module(m, w);
		m->pending_scale = true;
	}

	if (!m->pending_box) {
		draw_box_general(m->sx, m->sy, m->rx, m->ry, s);
		m->pending_box = true;
	}

	if (!m->pending_dividers) {
		r = draw_dividers(m, s);
		m->pending_dividers = true;
	}
*/

	for (int i = 0; i < m->nc; i++) {
		r = draw_column(m, w, s, i);
		if (r.rc != 0) return r;
		if (r.data != NULL) {
			if (*(int *)r.data > m->sx) {
				m->sx = *(int *)r.data;
			}
		}

	}


	fflush(stdout);

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

struct Result field_style(struct menu *m,
			   struct winsize *w,
			   struct style *s,
			   enum field_mode mode)
{
	struct Result r;

	struct column *cc = m->cs[m->cc];
	struct row *cr = cc->rs[cc->cr];
	struct field *f = (struct field *)cr->data;
/*
	int ypos;
	if (cc->cr > w->ws_row - 2) ypos = cc->ry + w->ws_row - 2;
	else ypos = cc->ry + cc->cr + 1;
*/
	switch (mode) {
		case TRAVERSE:
			f->mode = TRAVERSE;
			move(cr->rx, cr->ry);
			printf("\033[%d;%dm%*s\033[0m\n",
				s->text->foreground,
				s->text->background,
				f->name_len,
				f->name
			);
			break;
		case ENTRY:
			f->mode = ENTRY;
			r = init_field(m, w, s);
//			clear_column(m);

//			prompt_style(m, w, s, TRAVERSE);

//			draw_column(m, w, s, m->cc);
//			r = set_style(m, w, s);
			break;
	}

	fflush(stdout);

	r.rc = 0;
	return r;
}
/*
struct Result menu_style(struct menu_t *m, struct winsize *w)
{

}
*/
struct Result set_style(struct menu *m, struct winsize *w, struct style *s)
{
	// Handles default style case for row type
	// for menu-switch, etc.
	// More granular styling managed in directional (arrows,etc.) switch cases

	struct Result r;
	struct column *cc = m->cs[m->cc];

	if (cc->cr > w->ws_row - 2) {
		move(cc->rx + 1, cc->ry + w->ws_row - 2);
	} else {
		move(cc->rx + 2, cc->ry + cc->cr + 1);
	}


	switch (cc->rs[cc->cr]->type) {
		case MENU:
			printf(
				"\033[%d;%dm%*s\033[0m\n",
				s->text->foreground,
				s->text->background,
				m->sx,
				((struct menu *)cc->rs[cc->cr]->data)->pn
			);
			break;
		case FIELD:
			r = field_style(
				m,
				w,
				s,
				(enum field_mode){ TRAVERSE }
			); // update once done testing
//			wprintf(L"\033[30;47m%*s\033[0m\n", )

	//		wprintf(L"\033[30;47m%*s\033[0m\n", m->sx, ((struct menu_t *)cc->rs[cc->tr]->data)->pn);
		default:
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

struct Result clear_field_box(struct menu *m, struct style *s)
{
	struct Result r;

	struct column *cc = m->cs[m->cc];
	struct row *cr = cc->rs[cc->cr];
	struct field *f = cr->data;

	const int h = resolve_height(m);

	for (int i = 0; i < h; i++) {
		if (i == 0) {
			move(cr->rx + f->name_len + s->text_divider_len, cr->ry + i);
			for (int j = 0; j < cr->sx - (f->name_len + s->text_divider_len); j++) {
				printf(" ");
			}
		} else {
			move(cr->rx, cr->ry + i);
			for (int j = 0; j < cr->sx; j++) {
				printf(" ");
			}
		}
	}

	fflush(stdout);

	r.rc = 0;
	return r;
}

struct Result clear_box(struct menu *m, struct style *s)
{
	struct Result r;

	struct column *cc = m->cs[m->cc];
	struct row *cr = cc->rs[cc->cr];

	const int h = resolve_height(m);

	for (int i = 0; i < h; i++) {
		move(cc->rx + 1, cr->ry + i);
		for (int j = 0; j < cc->sx - 2; j++) {
			printf(" ");
		}
	}

	fflush(stdout);

	r.rc = 0;
	return r;
}

struct Result draw_box_aware(struct menu *m, struct style *s)
{
	struct Result r;

	struct column *cc = m->cs[m->cc];
	struct row *cr = cc->rs[cc->cr];

	const int h = resolve_height(m);

	r = draw_horizontal_bar(m, s, m->cc, cr->ry-1);
	r = draw_horizontal_bar(m, s, m->cc, cr->ry + h);

	fflush(stdout);

	r.rc = 0;
	return r;
}

// Should add explicite repositioning for top and bottom to make sure it's in
// the right place. Makes it more flexible if we can't rely on the newline
struct Result draw_box_general(int sx, int sy, int rx, int ry, struct style *s)
{
	struct Result r;

	printf("\033[%d;%dH", ry, rx);

	printf("%s", s->border->top_left);
	for (int i = 0; i < sx - 2; i++) {
		printf("%s", s->border->horizontal);
	}
	printf("%s", s->border->top_right);

	for (int i = 0; i < sy - 2; i++) {
		printf(
			"%s\033[%dC%s\n",
			s->border->vertical,
			sx,
			s->border->vertical
		);
	}	
/*
	if (rx != 1) {
		wprintf(L"\033[%dC", rx - 1);
	}
*/
	printf("%s", s->border->bottom_left);
	for (int i = 0; i < sx - 2; i++) {
		printf("%s", s->border->horizontal);
	}
	printf("%s", s->border->bottom_right);

	r.rc = 0;
	return r;

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
