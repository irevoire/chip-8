#include <stdio.h>
#include <stdlib.h>

#include "window.h"
#include "vm.h"

unsigned char chip8_fontset[] =
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

chip8_t *chip8_init()
{
	chip8_t *chip8 = malloc(sizeof(*chip8));
	if (chip8 == NULL)
		return NULL;

	/* initialize state */
	chip8->pc = 0x200;
	chip8->opcode = 0;
	chip8->I = 0;
	chip8->sp = 0;

	/* initialize timers */
	chip8->delay_timer = 0;
	chip8->sound_timer = 0;

	/* clear everything*/
	memset(chip8->memory, 0, sizeof(chip8->memory));
	memset(chip8->V, 0, sizeof(chip8->V));
	memset(chip8->stack, 0, sizeof(chip8->stack));
	memset(chip8->gfx, 0, sizeof(chip8->gfx));
	memset(chip8->key, 0, sizeof(chip8->key));

	/* load fontset into memory */
	for(size_t i = 0; i < sizeof(chip8_fontset); ++i)
		chip8->memory[i] = chip8_fontset[i];

	return chip8;
}

void chip8_free(chip8_t *chip8)
{
	if (chip8 != NULL)
		free(chip8);
}

int chip8_load_game(chip8_t *chip8, FILE *fd)
{
	// beginning of the game data
	unsigned char *game_buf = chip8->memory + 0x200; 
	size_t max_len = 0xEA0 - 0x200;

	if (chip8 == NULL || fd == NULL)
		return -1;

	fread(game_buf, max_len, 1, fd);
	if (ferror(fd))
		return -1;
	if (!feof(fd))
		return -1;
	return 0;
}


int chip8_emulate_cycle(chip8_t *chip8)
{
	uint16_t *opcode = &chip8->opcode;
	// get the current opcode
	memcpy(opcode, chip8->memory + chip8->pc, sizeof(*opcode));

	// Decode the opcode
	switch(*opcode & 0xF000)
	{
		case 0xA000: // ANNN: Sets I to the address NNN
			printf("Foud opcode\n");
			chip8->I = *opcode & 0x0FFF;
			chip8->pc += 2;
			break;
		default:
			printf("Unknown opcode: 0x%X\n", *opcode);
			return -1;
	}

	// Update timers
	if(chip8->delay_timer > 0)
		chip8->delay_timer--;

	if(chip8->sound_timer > 0)
	{
		if(chip8->sound_timer == 1)
			printf("BEEP!\n");
		chip8->sound_timer--;
	}
	return 0;
}
