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
			800, 600,
			SDL_WINDOW_RESIZABLE);
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

void window_clear(window_t *window)
{
	SDL_RenderClear(window->renderer);
}

void update_window(window_t *win, const unsigned char *gfx)
{
	int real_height, real_width;
	int h_f, w_f; // enlarge factor

	SDL_SetRenderDrawColor(win->renderer, 50, 50, 30, 255);
	SDL_RenderClear(win->renderer);
	SDL_GetWindowSize(win->window, &real_height, &real_width);

	h_f = real_height / win->h;
	w_f = real_width / win->w;

	printf("hf %d wf %d\n", h_f, w_f);

	for (int y = 0; y < win->h; y++)
	{
		for (int x = 0; x < win->w; x++)
		{
			//SDL_Rect rect = {x * h_f, y * h_f, h_f, w_f};
			SDL_Rect rect = {x * 5, y * 5, 5, 5};

			if (gfx[x + y * 64] == 0)
				continue;
			SDL_SetRenderDrawColor(win->renderer, 230, 255, 255, 255);
			SDL_RenderFillRect(win->renderer, &rect);
		}
	}
	SDL_RenderPresent(win->renderer);

}

static void handle_keyboard(unsigned char *keyboard, int sym, unsigned char value)
{
	switch (sym)
	{
		case SDLK_1:
			keyboard[0] = value;
			break;
		case SDLK_2:
			keyboard[1] = value;
			break;
		case SDLK_3:
			keyboard[2] = value;
			break;
		case SDLK_4:
			keyboard[3] = value;
			break;
		case SDLK_a:
			keyboard[4] = value;
			break;
		case SDLK_z:
			keyboard[5] = value;
			break;
		case SDLK_e:
			keyboard[6] = value;
			break;
		case SDLK_r:
			keyboard[7] = value;
			break;
		case SDLK_q:
			keyboard[8] = value;
			break;
		case SDLK_s:
			keyboard[9] = value;
			break;
		case SDLK_d:
			keyboard[10] = value;
			break;
		case SDLK_f:
			keyboard[11] = value;
			break;
		case SDLK_w:
			keyboard[12] = value;
			break;
		case SDLK_x:
			keyboard[13] = value;
			break;
		case SDLK_c:
			keyboard[14] = value;
			break;
		case SDLK_v:
			keyboard[15] = value;
			break;
	}
}

int handle_event(unsigned char *keyboard)
{
	SDL_Event event;

	while(SDL_PollEvent(&event) != 0)
	{
		switch (event.type)
		{
			case SDL_QUIT:
				return 1;
			case SDL_KEYDOWN:
				handle_keyboard(keyboard, event.key.keysym.sym, 1);
				break;
			case SDL_KEYUP:
				handle_keyboard(keyboard, event.key.keysym.sym, 0);
				break;
			default:
				/* "Event not yet implemented */
				break;
		}
	}

	return 0;
}

