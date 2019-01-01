#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <SDL.h>

typedef struct window_s {
	SDL_Window *window;
	SDL_Renderer *renderer;

	int w;
	int h;
} window_t;

window_t *create_window(int width, int height);

void destroy_window(window_t *window);

void window_clear(window_t *window);

#endif /* _WINDOW_H_ */
