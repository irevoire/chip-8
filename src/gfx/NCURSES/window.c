#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ncurses.h>

#include "window.h"

struct window_s {
	int w;
	int h;
};

window_t *create_window(int width, int height)
{
	struct window_s *window = malloc(sizeof(*window));

	window->w = width;
	window->h = height;

	initscr();
	timeout(0);

	return window;
}

void destroy_window(window_t *window)
{
	// restore the old terminal settings
	endwin();

	free(window);
}

void window_clear(window_t *window)
{
	(void)window;
	clear();
	refresh();
}

void update_window(window_t *window, const unsigned char *gfx)
{
	struct window_s *win = window;

	for (int y = 0; y < win->h; y++)
	{
		for (int x = 0; x < win->w; x++)
		{
			if (gfx[x + y * 64] == 0)
			{
				move(y, x);
				addch(' ');
			}
			else
			{
				move(y, x);
				addch('X');
			}
		}
	}
	refresh();
}

int handle_event(unsigned char *keyboard)
{
	// clear the keyboard before adding the new input
	memset(keyboard, 0, 16);

	switch (getch())
	{
		// C^c
		case 3:
		// C^c
		case 26:
			return 1;
		case '1':
			keyboard[0] = 1;
			break;
		case '2':
			keyboard[1] = 1;
			break;
		case '3':
			keyboard[2] = 1;
			break;
		case '4':
			keyboard[3] = 1;
			break;
		case 'a':
		case 'A':
			keyboard[4] = 1;
			break;
		case 'z':
		case 'Z':
			keyboard[5] = 1;
			break;
		case 'e':
		case 'E':
			keyboard[6] = 1;
			break;
		case 'r':
		case 'R':
			keyboard[7] = 1;
			break;
		case 'q':
		case 'Q':
			keyboard[8] = 1;
			break;
		case 's':
		case 'S':
			keyboard[9] = 1;
			break;
		case 'd':
		case 'D':
			keyboard[10] = 1;
			break;
		case 'f':
		case 'F':
			keyboard[11] = 1;
			break;
		case 'w':
		case 'W':
			keyboard[12] = 1;
			break;
		case 'x':
		case 'X':
			keyboard[13] = 1;
			break;
		case 'c':
		case 'C':
			keyboard[14] = 1;
			break;
		case 'v':
		case 'V':
			keyboard[15] = 1;
			break;
	}
	return 0;
}

