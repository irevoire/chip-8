#ifndef _SDL_UTILS_H_
#define _SDL_UTILS_H_

#define handle_SDL_Error(s) \
	do { \
		SDL_Log("%s: %s", (s), SDL_GetError()); \
		exit(EXIT_FAILURE); \
	} while (0);

#endif /* _SDL_UTILS_H_ */
