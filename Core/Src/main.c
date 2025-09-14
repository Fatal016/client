#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <string.h>
#include <wchar.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>

#include "../Inc/tui.h"
#include "../Inc/menu.h"
#include "../Inc/templates.h"

char buf[2048];

int main(int argc, char** argv)
{
	struct Result r;
	struct winsize w;
	struct menu_t *menu = &main_menu;

	/* Initialization */
	setlocale(LC_CTYPE, "");
	signal(SIGINT, handle_signal);
	set_noncanonical_mode();
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	wprintf(HIDE_CURSOR);

	wprintf(CLEAR_DISPLAY);
	r = draw_module(menu, &w);
	r = set_style(menu, &w);

	while(1) {
		r = menu_switch(&menu, &w);
	}

	return 0;
}

/*
		switch(ch) {
			case UP_ARROW:
				clear_style(active_menu, &w);
				if (active_menu->size_y > w.ws_row && active_menu->cur_y >= w.ws_row) {
					active_menu->cur_y--;
					active_menu->item_offset--;
				} else {
					if (active_menu->cur_y == 1) {
						active_menu->cur_y = active_menu->size_y;
						if (active_menu->size_y > w.ws_row) {
							active_menu->item_offset = active_menu->size_y - w.ws_row + 2;
						} else {
							active_menu->item_offset = 0;
						}	
					} else {
						active_menu->cur_y--;
						active_menu->item_offset = 0;
					}
				}

				wprintf(L"\033[2J\033[H");
				if (active_menu->type == FIELD) {
					draw_field(active_menu);
				} else {
					draw_menu(active_menu);
				}
				set_style(active_menu, &w);
				wprintf(L"\033[0m");
				break;
			case DOWN_ARROW:
				if (active_menu->size_y > w.ws_row && active_menu->cur_y >= w.ws_row - 2 && active_menu->cur_y <= active_menu->size_y) {
					clear_style(active_menu, &w);
					if (active_menu->cur_y == active_menu->size_y) {
						active_menu->item_offset = 0;
						active_menu->cur_y = 1;
					} else {
						active_menu->cur_y++;
						active_menu->item_offset++;
					}

					wprintf(L"\033[2J\033[H");
					draw_field(active_menu);
					set_style(active_menu, &w);
					wprintf(L"\033[0m");
				} else {
					active_menu->item_offset = 0;
					clear_style(active_menu, &w);
					if (active_menu->cur_y == active_menu->size_y) {
						active_menu->cur_y = 1;
					} else {	
						active_menu->cur_y++;
					}
					set_style(active_menu, &w);
				}
				break;	
			case RIGHT_ARROW:
				if (active_menu->type == MENU) {			
					active_menu = active_menu->items[active_menu->cur_y - 1];
				
					wprintf(L"\033[0m");
					wprintf(L"\033[2J\033[H");
					
					if (active_menu->type == MENU) {
						draw_menu(active_menu);
					} else if (active_menu->type == FIELD) {
						draw_field(active_menu);
					}

					//	active_menu->cur_y = 1;
					set_style(active_menu, &w);
				}	
				break;
			case LEFT_ARROW:
				if (active_menu->prev_menu != NULL) {	
					active_menu = active_menu->prev_menu;

					wprintf(L"\033[0m");
					wprintf(L"\033[2J\033[H");

					draw_menu(active_menu);
					set_style(active_menu, &w);
				}
				break;
			default:
			//	wprintf(L"%ld\n", ch);
				break;
		}

		fflush(stdout);
	*/

struct Result menu_r_arrow(struct menu_t **m, struct winsize *w)
{
	struct Result r;
	struct column_t *cc = (*m)->cs[(*m)->cc];

	switch(cc->rs[cc->tr]->type) {
		case MENU:
			wprintf(L"\033[2J\033[H");
			*m = (struct menu_t *)(cc->rs[cc->tr]->data);
			r = draw_module(*m, w);
			set_style(*m, w);
			break;

//			r.data = (void*)malloc(sizeof(struct menu_t*));
//			*(struct menu_t*)(r.data) = active_menu;
		case FIELD:
			break;
		case PROMPT:
			break;
	}

	r.rc = 0;


	return r;
}

struct Result menu_l_arrow(struct menu_t **m, struct winsize *w)
{
	struct Result r;
	struct column_t *cc = (*m)->cs[(*m)->cc];

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

	switch(cc->rs[cc->tr]->type) {
		case PROMPT:
			// Want to avoid clearing screen completely
			struct prompt_t *p = (struct prompt_t *)cc->rs[cc->tr]->data;
			if (p->mode == TRAVERSE) {
				//wprintf(L"\033[2J\033[H");
				r = prompt_style(*m, w, ENTRY);
			}
			break;
	}

	r.rc = 0;
	return r;
}

struct Result init_prompt(struct menu_t *m, struct winsize *w)
{
	struct Result r;
	struct column_t *cc = m->cs[m->cc];
	struct prompt_t *p = (struct prompt_t *)cc->rs[cc->tr]->data;

	int ypos;
	if (cc->tr > w->ws_row - 2) ypos = cc->ry + w->ws_row - 2;
	else ypos = cc->ry + cc->tr + 1;
		
	int offset = ((struct prompt_t *)(cc->rs[cc->tr]->data))->name_len + 1;
	moveCursor(cc->rx + 2 + offset, ypos);

	if (p->value == p->placeholder) {
		wprintf(L"\033[0m%*s\033[0m\n", p->value_len, L" ");
		moveCursor(cc->rx + 2 + offset, ypos);
	} else {
		moveCursor(cc->rx + 2 + offset + p->value_len, ypos);
	}


	wprintf(L"\033[47m \033[0m");

	// Need conditional to check mem location that value is pointed at

	r.rc = 0;
	return r;
}

void moveCursor(int x, int y)
{
	wprintf(L"\033[%d;%dH", y, x);
}

void set_noncanonical_mode(void) {
	struct termios term;
	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag &= ~(ICANON | ECHO);
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
