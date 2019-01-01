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

	// Set up render system and register input callbacks
	create_window(64, 32);
//	setupInput();

	// Initialize the Chip8 system and load the game into the memory
	chip8 = chip8_init();
	chip8_load_game(chip8, fd);

	// Emulation loop
	while(1)
	{
		// Emulate one cycle
		if (chip8_emulate_cycle(chip8))
			break; // error

		/*
		// If the draw flag is set, update the screen
		if(myChip8.drawFlag)
			drawGraphics();

		// Store key press state (Press and Release)
		myChip8.setKeys();
		*/
	}

	chip8_free(chip8);

	return 0;
}
