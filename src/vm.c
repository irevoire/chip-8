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

static void chip8_handle_opcode(chip8_t *chip8);

int chip8_emulate_cycle(chip8_t *chip8)
{
	chip8_handle_opcode(chip8);

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

static void chip8_opcode_0NNN(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_00E0(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_00EE(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_1NNN(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_2NNN(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_3XNN(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_4XNN(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_5XY0(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_6XNN(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_7XNN(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_8XY0(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_8XY1(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_8XY2(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_8XY3(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_8XY4(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_8XY5(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_8XY6(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_8XY7(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_8XYE(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_9XY0(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_ANNN(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_BNNN(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_CXNN(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_DXYN(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_EX9E(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_EXA1(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_FX07(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_FX0A(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_FX15(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_FX18(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_FX1E(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_FX29(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_FX33(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_FX55(chip8_t *chip8, uint16_t *opcode);
static void chip8_opcode_FX65(chip8_t *chip8, uint16_t *opcode);

static void chip8_handle_opcode(chip8_t *chip8)
{
	uint16_t *opcode = &chip8->opcode;
	// get the current opcode
	memcpy(opcode, chip8->memory + chip8->pc, sizeof(*opcode));

	// Decode the opcode
	if (*opcode == 0x00EE)
		chip8_opcode_00EE(chip8, opcode);
	else if (*opcode == 0x00E0)
		chip8_opcode_00E0(chip8, opcode);
	else if ((*opcode & 0xF000) == 0x0000)
		chip8_opcode_0NNN(chip8, opcode);
	else if ((*opcode & 0xF000) == 0x1000)
		chip8_opcode_1NNN(chip8, opcode);
	else if ((*opcode & 0xF000) == 0x2000)
		chip8_opcode_2NNN(chip8, opcode);
	else if ((*opcode & 0xF000) == 0x3000)
		chip8_opcode_3XNN(chip8, opcode);
	else if ((*opcode & 0xF000) == 0x4000)
		chip8_opcode_4XNN(chip8, opcode);
	else if ((*opcode & 0xF00F) == 0x5000)
		chip8_opcode_5XY0(chip8, opcode);
	else if ((*opcode & 0xF000) == 0x6000)
		chip8_opcode_6XNN(chip8, opcode);
	else if ((*opcode & 0xF000) == 0x7000)
		chip8_opcode_7XNN(chip8, opcode);
	else if ((*opcode & 0xF00F) == 0x8000)
		chip8_opcode_8XY0(chip8, opcode);
	else if ((*opcode & 0xF00F) == 0x8001)
		chip8_opcode_8XY1(chip8, opcode);
	else if ((*opcode & 0xF00F) == 0x8002)
		chip8_opcode_8XY2(chip8, opcode);
	else if ((*opcode & 0xF00F) == 0x8003)
		chip8_opcode_8XY3(chip8, opcode);
	else if ((*opcode & 0xF00F) == 0x8004)
		chip8_opcode_8XY4(chip8, opcode);
	else if ((*opcode & 0xF00F) == 0x8005)
		chip8_opcode_8XY5(chip8, opcode);
	else if ((*opcode & 0xF00F) == 0x8006)
		chip8_opcode_8XY6(chip8, opcode);
	else if ((*opcode & 0xF00F) == 0x8007)
		chip8_opcode_8XY7(chip8, opcode);
	else if ((*opcode & 0xF00F) == 0x800E)
		chip8_opcode_8XYE(chip8, opcode);
	else if ((*opcode & 0xF00F) == 0x9000)
		chip8_opcode_9XY0(chip8, opcode);
	else if ((*opcode & 0xF000) == 0xA000)
		chip8_opcode_ANNN(chip8, opcode);
	else if ((*opcode & 0xF000) == 0xB000)
		chip8_opcode_BNNN(chip8, opcode);
	else if ((*opcode & 0xF000) == 0xC000)
		chip8_opcode_CXNN(chip8, opcode);
	else if ((*opcode & 0xF000) == 0xD000)
		chip8_opcode_DXYN(chip8, opcode);
	else if ((*opcode & 0xF0FF) == 0xE09E)
		chip8_opcode_EX9E(chip8, opcode);
	else if ((*opcode & 0xF0FF) == 0xE0A1)
		chip8_opcode_EXA1(chip8, opcode);
	else if ((*opcode & 0xF0FF) == 0xF007)
		chip8_opcode_FX07(chip8, opcode);
	else if ((*opcode & 0xF0FF) == 0xF00A)
		chip8_opcode_FX0A(chip8, opcode);
	else if ((*opcode & 0xF0FF) == 0xF015)
		chip8_opcode_FX15(chip8, opcode);
	else if ((*opcode & 0xF0FF) == 0xF018)
		chip8_opcode_FX18(chip8, opcode);
	else if ((*opcode & 0xF0FF) == 0xF01E)
		chip8_opcode_FX29(chip8, opcode);
	else if ((*opcode & 0xF0FF) == 0xF033)
		chip8_opcode_FX33(chip8, opcode);
	else if ((*opcode & 0xF0FF) == 0xF055)
		chip8_opcode_FX55(chip8, opcode);
	else if ((*opcode & 0xF0FF) == 0xF065)
		chip8_opcode_FX65(chip8, opcode);
	else
	{
		printf("Unknown opcode: 0x%.4X\n", *opcode);
		exit(1);
	}

	/*
	switch(*opcode & 0xF000)
	{
		case 0xA000: 
			chip8_opcode_ANNN(chip8, opcode);
			break;
		default:
	}
	*/
}

/* Calls RCA 1802 program at address NNN. Not necessary for most ROMs. */
static void chip8_opcode_0NNN(chip8_t *chip8, uint16_t *opcode)
{
	// remove ununsed parameters warnings
	(void) chip8;
	(void) opcode;
}

/* Clears the screen. */
static void chip8_opcode_00E0(chip8_t *chip8, uint16_t *opcode)
{
	// remove ununsed parameters warnings
	(void) opcode;

	window_clear(chip8->window);
}

/* */
static void chip8_opcode_00EE(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_1NNN(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_2NNN(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_3XNN(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_4XNN(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_5XY0(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_6XNN(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_7XNN(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_8XY0(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_8XY1(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_8XY2(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_8XY3(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_8XY4(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_8XY5(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_8XY6(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_8XY7(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_8XYE(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_9XY0(chip8_t *chip8, uint16_t *opcode)
{

}


// Sets I to the address NNN
static void chip8_opcode_ANNN(chip8_t *chip8, uint16_t *opcode)
{
	printf("Foud ANNN\n");
	chip8->I = *opcode & 0x0FFF;
	chip8->pc += 2;
}

/* */
static void chip8_opcode_BNNN(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_CXNN(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_DXYN(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_EX9E(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_EXA1(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_FX07(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_FX0A(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_FX15(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_FX18(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_FX1E(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_FX29(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_FX33(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_FX55(chip8_t *chip8, uint16_t *opcode)
{

}

/* */
static void chip8_opcode_FX65(chip8_t *chip8, uint16_t *opcode)
{

}

