#include <stdio.h>
#include <stdlib.h>

#include "window.h"
#include "vm.h"

const unsigned char chip8_fontset[] =
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

	*opcode = (uint16_t) (chip8->memory[chip8->pc] << 8 | chip8->memory[chip8->pc + 1]);

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
		chip8_opcode_FX1E(chip8, opcode);
	else if ((*opcode & 0xF0FF) == 0xF029)
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

	memset(chip8->gfx, 0, sizeof(chip8->gfx));

	window_clear(chip8->window);
}

/* Returns from a subroutine. */
static void chip8_opcode_00EE(chip8_t *chip8, uint16_t *opcode)
{
	// remove ununsed parameters warnings
	(void) opcode;

	chip8->sp--;
	chip8->pc = chip8->stack[chip8->sp];
	chip8->pc += 2;
}

/* Jumps to address NNN. */
static void chip8_opcode_1NNN(chip8_t *chip8, uint16_t *opcode)
{
	chip8->pc = *opcode & 0x0FFF;
}

/* Calls subroutine at NNN. */
static void chip8_opcode_2NNN(chip8_t *chip8, uint16_t *opcode)
{
	// Store current address in stack
	chip8->stack[chip8->sp] = chip8->pc;
	chip8->sp++; // Increment stack pointer
	// Set the program counter to the address at NNN
	chip8->pc = *opcode & 0x0FFF;
}

/* Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block) */
static void chip8_opcode_3XNN(chip8_t *chip8, uint16_t *opcode)
{
	if(chip8->V[(*opcode & 0x0F00) >> 8] == (*opcode & 0x00FF))
		chip8->pc += 4;
	else
		chip8->pc += 2;
}

/* Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block) */
static void chip8_opcode_4XNN(chip8_t *chip8, uint16_t *opcode)
{
	if(chip8->V[(*opcode & 0x0F00) >> 8] != (*opcode & 0x00FF))
		chip8->pc += 4;
	else
		chip8->pc += 2;
}

/* Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block) */
static void chip8_opcode_5XY0(chip8_t *chip8, uint16_t *opcode)
{
	if(chip8->V[(*opcode & 0x0F00) >> 8] // VX
			== chip8->V[(*opcode & 0x00F0) >> 4]) // VY
		chip8->pc += 4;
	else
		chip8->pc += 2;
}

/* Sets VX to NN. */
static void chip8_opcode_6XNN(chip8_t *chip8, uint16_t *opcode)
{
	chip8->V[(*opcode & 0x0F00) >> 8] = *opcode & 0x00FF;
	chip8->pc += 2;
}

/* Adds NN to VX. (Carry flag is not changed) */
static void chip8_opcode_7XNN(chip8_t *chip8, uint16_t *opcode)
{
	chip8->V[(*opcode & 0x0F00) >> 8] += *opcode & 0x00FF;
	chip8->pc += 2;
}

/* Sets VX to the value of VY. */
static void chip8_opcode_8XY0(chip8_t *chip8, uint16_t *opcode)
{
	unsigned char X = (*opcode & 0x0F00) >> 8;
	unsigned char Y = (*opcode & 0x00F0) >> 4;

	chip8->V[X] = chip8->V[Y];
	chip8->pc += 2;
}

/* Sets VX to VX or VY. (Bitwise OR operation) */
static void chip8_opcode_8XY1(chip8_t *chip8, uint16_t *opcode)
{
	unsigned char X = (*opcode & 0x0F00) >> 8;
	unsigned char Y = (*opcode & 0x00F0) >> 4;

	chip8->V[X] |= chip8->V[Y];
	chip8->pc += 2;
}

/* Sets VX to VX and VY. (Bitwise AND operation) */
static void chip8_opcode_8XY2(chip8_t *chip8, uint16_t *opcode)
{
	unsigned char X = (*opcode & 0x0F00) >> 8;
	unsigned char Y = (*opcode & 0x00F0) >> 4;

	chip8->V[X] &= chip8->V[Y];
	chip8->pc += 2;
}

/* Sets VX to VX xor VY. */
static void chip8_opcode_8XY3(chip8_t *chip8, uint16_t *opcode)
{
	unsigned char X = (*opcode & 0x0F00) >> 8;
	unsigned char Y = (*opcode & 0x00F0) >> 4;

	chip8->V[X] ^= chip8->V[Y];
	chip8->pc += 2;
}

/* Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't. */
static void chip8_opcode_8XY4(chip8_t *chip8, uint16_t *opcode)
{
	unsigned char X = (*opcode & 0x0F00) >> 8;
	unsigned char Y = (*opcode & 0x00F0) >> 4;

	if(chip8->V[Y] > (0xFF - chip8->V[X]))
		chip8->V[0xF] = 1; //carry
	else
		chip8->V[0xF] = 0;
	chip8->V[X] += chip8->V[Y];
	chip8->pc += 2;
}

/* VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't. */
static void chip8_opcode_8XY5(chip8_t *chip8, uint16_t *opcode)
{
	unsigned char X = (*opcode & 0x0F00) >> 8;
	unsigned char Y = (*opcode & 0x00F0) >> 4;

	if(chip8->V[X] > chip8->V[Y]) 
		chip8->V[0xF] = 0; // there is a borrow
	else 
		chip8->V[0xF] = 1;					
	chip8->V[Y] -= chip8->V[X];
	chip8->pc += 2;
}

/* Stores the least significant bit of VX in VF and then shifts VX to the right by 1. */
static void chip8_opcode_8XY6(chip8_t *chip8, uint16_t *opcode)
{
	unsigned char X = (*opcode & 0x0F00) >> 8;

	chip8->V[0xF] = chip8->V[X] & 0x1;
	chip8->V[X] >>= 1;
	chip8->pc += 2;
}

/* Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't. */
static void chip8_opcode_8XY7(chip8_t *chip8, uint16_t *opcode)
{
	unsigned char X = (*opcode & 0x0F00) >> 8;
	unsigned char Y = (*opcode & 0x00F0) >> 4;

	if(chip8->V[X] > chip8->V[Y])
		chip8->V[0xF] = 0;
	else
		chip8->V[0xF] = 1;

	chip8->V[X] = chip8->V[Y] - chip8->V[X];
	chip8->pc += 2;
}

/* Stores the most significant bit of VX in VF and then shifts VX to the left by 1. */
static void chip8_opcode_8XYE(chip8_t *chip8, uint16_t *opcode)
{
	unsigned char X = (*opcode & 0x0F00) >> 8;

	chip8->V[0xF] = chip8->V[X] >> 7;
	chip8->V[X] <<= 1;
	chip8->pc += 2;
}

/* Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block) */
static void chip8_opcode_9XY0(chip8_t *chip8, uint16_t *opcode)
{
	unsigned char X = (*opcode & 0x0F00) >> 8;
	unsigned char Y = (*opcode & 0x00F0) >> 4;

	if(chip8->V[X] != chip8->V[Y])
		chip8->pc += 4;
	else
		chip8->pc += 2;
}

// Sets I to the address NNN
static void chip8_opcode_ANNN(chip8_t *chip8, uint16_t *opcode)
{
	chip8->I = *opcode & 0x0FFF;
	chip8->pc += 2;
}

/* Jumps to the address NNN plus V0. */
static void chip8_opcode_BNNN(chip8_t *chip8, uint16_t *opcode)
{
	chip8->pc = (*opcode & 0x0FFF) + chip8->V[0];
}

/* Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN. */
static void chip8_opcode_CXNN(chip8_t *chip8, uint16_t *opcode)
{
	unsigned char X = (*opcode & 0x0F00) >> 8;

	chip8->V[X] = (rand() % 0xFF) & (*opcode & 0x00FF);
	chip8->pc += 2;
}

/* Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value doesn’t change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t happen */
static void chip8_opcode_DXYN(chip8_t *chip8, uint16_t *opcode)
{
	unsigned char X = chip8->V[(*opcode & 0x0F00) >> 8];
	unsigned char Y = chip8->V[(*opcode & 0x00F0) >> 4];
	uint16_t height = *opcode & 0x000F;
	uint16_t pixel;

	chip8->V[0xF] = 0;
	for (int s_y = 0; s_y < height; s_y++)
	{
		pixel = chip8->memory[chip8->I + s_y];
		for(int s_x = 0; s_x < 8; s_x++)
		{
			if((pixel & (0x80 >> s_x)) != 0)
			{
				if(chip8->gfx[(X + s_x + ((Y + s_y) * 64))] == 1)
					chip8->V[0xF] = 1;
				chip8->gfx[X + s_x + ((Y + s_y) * 64)] ^= 1;
			}
		}
	}
	chip8->pc += 2;
}

/* Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block) */
static void chip8_opcode_EX9E(chip8_t *chip8, uint16_t *opcode)
{
	unsigned char X = (*opcode & 0x0F00) >> 8;

	if(chip8->key[chip8->V[X]] != 0)
		chip8->pc += 4;
	else
		chip8->pc += 2;
}

/* Skips the next instruction if the key stored in VX isn't pressed. (Usually the next instruction is a jump to skip a code block) */
static void chip8_opcode_EXA1(chip8_t *chip8, uint16_t *opcode)
{
	unsigned char X = (*opcode & 0x0F00) >> 8;

	if(chip8->key[chip8->V[X]] == 0)
		chip8->pc += 4;
	else
		chip8->pc += 2;
}

/* Sets VX to the value of the delay timer. */
static void chip8_opcode_FX07(chip8_t *chip8, uint16_t *opcode)
{
	unsigned char X = (*opcode & 0x0F00) >> 8;

	chip8->V[X] = chip8->delay_timer;
	chip8->pc += 2;
}

/* A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event) */
static void chip8_opcode_FX0A(chip8_t *chip8, uint16_t *opcode)
{
	unsigned char X = (*opcode & 0x0F00) >> 8;

	char key_pressed = 0;

	for(unsigned char i = 0; i < 16; i++)
	{
		if(chip8->key[i] != 0)
		{
			chip8->V[X] = i;
			key_pressed = 1;
		}
	}

	// If we didn't received a keypress, skip this cycle and try again.
	if(key_pressed == 0)
		return;

	chip8->pc += 2;
}

/* Sets the delay timer to VX. */
static void chip8_opcode_FX15(chip8_t *chip8, uint16_t *opcode)
{
	unsigned char X = (*opcode & 0x0F00) >> 8;

	chip8->delay_timer = chip8->V[X];
	chip8->pc += 2;
}

/* Sets the sound timer to VX. */
static void chip8_opcode_FX18(chip8_t *chip8, uint16_t *opcode)
{
	unsigned char X = (*opcode & 0x0F00) >> 8;

	chip8->sound_timer = chip8->V[X];
	chip8->pc += 2;
}

/* Adds VX to I. */
static void chip8_opcode_FX1E(chip8_t *chip8, uint16_t *opcode)
{
	unsigned char X = (*opcode & 0x0F00) >> 8;

	if(chip8->I + chip8->V[X] > 0xFFF)
		chip8->V[0xF] = 1;
	else
		chip8->V[0xF] = 0;
	chip8->I += chip8->V[X];
	chip8->pc += 2;
}

/* Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font. */
static void chip8_opcode_FX29(chip8_t *chip8, uint16_t *opcode)
{
	unsigned char X = (*opcode & 0x0F00) >> 8;

	chip8->I = chip8->V[X] * 0x5;
	chip8->pc += 2;
}

/* Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2. (In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.) */
static void chip8_opcode_FX33(chip8_t *chip8, uint16_t *opcode)
{
	unsigned char X = (*opcode & 0x0F00) >> 8;

	chip8->memory[chip8->I]     = chip8->V[X] / 100;
	chip8->memory[chip8->I + 1] = (chip8->V[X] / 10) % 10;
	chip8->memory[chip8->I + 2] = chip8->V[X] % 10;
	chip8->pc += 2;
}

/* Stores V0 to VX (including VX) in memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified. */
static void chip8_opcode_FX55(chip8_t *chip8, uint16_t *opcode)
{
	unsigned char X = (*opcode & 0x0F00) >> 8;

	for (unsigned char i = 0; i <= X; i++)
		chip8->memory[chip8->I + i] = chip8->V[i];	

	chip8->I += X + 1; // useless ?
	chip8->pc += 2;
}

/* Fills V0 to VX (including VX) with values from memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified. */
static void chip8_opcode_FX65(chip8_t *chip8, uint16_t *opcode)
{
	unsigned char X = (*opcode & 0x0F00) >> 8;

	for (unsigned char i = 0; i <= X; i++)
		chip8->V[i] = chip8->memory[chip8->I + i];

	chip8->I += X + 1; // useless ?
	chip8->pc += 2;
}
