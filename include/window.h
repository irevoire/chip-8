#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <SDL.h>

struct window_s {
	SDL_Window *window;
	SDL_Renderer *renderer;

	int w;
	int h;
};

typedef struct window_s* window_t;

extern window_t window;

void create_window(int width, int height);

void destroy_window(void);

#endif /* _WINDOW_H_ */
