#include <stdio.h>
#include <unistd.h>
#include <locale.h>
#include <signal.h>

#include "../include/tui.h"
#include "../include/menu.h"
#include "../include/templates.h"
#include "../include/style.h"

int main(int argc, char** argv)
{
	struct Result result;
	struct winsize w;
	struct menu *m = &search_menu;

	struct style *s = style_init(
				&BORDER_SINGLE,
				&BLACK_AND_WHITE,
				": ",
				0,
				0
	);

	/* Initialization */
	setlocale(LC_CTYPE, "");
	signal(SIGINT, handle_signal);
	set_noncanonical_mode(0);
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	printf(CURSOR_HIDE);
	
	printf(CLEAR_DISPLAY);
	result = draw_module(m, &w, s);

	struct row *r = m->cs[m->cc]->rs[m->cs[m->cc]->cr];
	result = set_style(r, s);

	while(1) {
		result = menu_switch(&m, &w, s);
	}

	return result.rc;
}
