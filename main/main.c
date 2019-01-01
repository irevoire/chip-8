#include <stdio.h>
#include <stdlib.h>

#include "vm.h"
#include "window.h"

chip8_t *chip8;

static void usage(void)
{
	fprintf(stderr, "usage: %s game_file\n", __FILE__);
	exit(1);
}

int main(int argc, char **argv)
{
	FILE *fd = NULL;
	// Get a filedescriptor to the game
	if (argc == 1)
		fd = stdin;
	else if (argc == 2)
	{
		fd = fopen(argv[1], "rb");
		if (fd == NULL)
		{
			perror("Failed to open file: ");
			return 0;
		}
	}
	else
		usage();

	// Initialize the Chip8 system
	chip8 = chip8_init();

	// Set up render system and register input callbacks
	chip8->window = create_window(640, 320);

	SDL_SetRenderDrawColor(chip8->window->renderer, 255, 0, 0, 255);

	// Load the game into the memory
	chip8_load_game(chip8, fd);

	// Emulation loop
	while(1)
	{
		// Emulate one cycle
		if (chip8_emulate_cycle(chip8))
			break; // error

		update_window(chip8->window, chip8->gfx);

		// Store key press state (Press and Release)
		if (handle_event(chip8->key))
			break;
	}

	chip8_free(chip8);

	return 0;
}
