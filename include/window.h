#ifndef _WINDOW_H_
#define _WINDOW_H_

typedef void window_t;

window_t *create_window(int width, int height);

void destroy_window(window_t *window);

void window_clear(window_t *window);

void update_window(window_t *win, const unsigned char *gfx);

int handle_event(unsigned char *keyboard);

#endif /* _WINDOW_H_ */
