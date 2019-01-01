#include <SDL.h>

#include "window.h"
#include "SDL_utils.h"

window_t *create_window(int width, int height)
{
	window_t *window = malloc(sizeof(*window));
	if (SDL_Init(SDL_INIT_VIDEO))
		handle_SDL_Error("Unable to initialize SDL");

	window->window = SDL_CreateWindow("CHIP-8",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			width, height,
			0);
	if (window->window == NULL)
		handle_SDL_Error("Could not create window");

	window->renderer = SDL_CreateRenderer(window->window, -1,
			SDL_RENDERER_ACCELERATED |
			SDL_RENDERER_PRESENTVSYNC |
			SDL_RENDERER_TARGETTEXTURE);
	if (window->renderer == NULL)
		handle_SDL_Error("Could not create render");

	window->w = width;
	window->h = height;

	return window;
}

void destroy_window(window_t *window)
{
	SDL_DestroyRenderer(window->renderer);
	SDL_DestroyWindow(window->window);

	free(window);
}

static int handle_event()
{
	SDL_Event event;

	while(SDL_PollEvent(&event) != 0)
	{
		switch (event.type)
		{
			case SDL_QUIT:
				return 1;
			case SDL_KEYDOWN:
				break;
			default:
				/* "Event not yet implemented */
				break;
		}
	}

	return 0;
}

void window_clear(window_t *window)
{
	SDL_RenderClear(window->renderer);
}
