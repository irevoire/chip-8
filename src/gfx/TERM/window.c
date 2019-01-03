#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#include "window.h"

struct window_s {
	struct termios old_t;
	struct termios new_t;

	int w;
	int h;
};

window_t *create_window(int width, int height)
{
	struct window_s *window = malloc(sizeof(*window));

	window->w = width;
	window->h = height;

	// print an empty screen
	for (; height > 0; height--)
		printf("\n");

	// save the current state of the terminal to restore it later
	tcgetattr(STDIN_FILENO, &(window->old_t));

	window->new_t = window->old_t; // copy the current settings
	// we get input right when typed
	window->new_t.c_lflag &= (unsigned long)~ICANON;
	// nothing is printted when typing
	window->new_t.c_lflag &= (unsigned long)~ECHO;
//	window->new_t.c_lflag &= (unsigned long)~ISIG;
	window->new_t.c_cc[VMIN] = 0;
	window->new_t.c_cc[VTIME] = 0;

	// apply the new settings
	tcsetattr(STDIN_FILENO, TCSANOW, &(window->new_t));

	return window;
}

void destroy_window(window_t *window)
{
	struct window_s *win = window;

	// restore the old settings
	tcsetattr(STDIN_FILENO, TCSANOW, &(win->old_t));

	free(window);
}

void window_clear(window_t *window)
{
	struct window_s *win = window;
	for (unsigned char h = 0; h < win->h; h++)
		printf("\e[A\e[K");
	printf("\e[%dB", win->h);
}

void update_window(window_t *window, const unsigned char *gfx)
{
	struct window_s *win = window;

	printf("\e[%dA", win->h);
	for (int y = 0; y < win->h; y++)
	{
		for (int x = 0; x < win->w; x++)
		{
			if (gfx[x + y * 64] == 0)
				printf(" ");
			else
				printf("X");
		}
		printf("\n");
	}

}

int handle_event(unsigned char *keyboard)
{
	// clear the keyboard before adding the new input
	memset(keyboard, 0, 16);

	switch (getchar())
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

