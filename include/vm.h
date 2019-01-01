#ifndef _VM_H_
#define _VM_H_

#include <stdint.h>
#include <stdio.h>

#include "window.h"

extern const unsigned char chip8_fontset[];

typedef struct chip8_s {
	uint16_t opcode; /* all the instruction are on two bytes */
	unsigned char memory[0x1000]; /* 4ko for the chip8 */
	unsigned char V[16]; /* 15 register + one carry flag*/
	uint16_t I; /* index register */
	uint16_t pc; /* program counter */
	uint16_t stack[16];
	uint16_t sp; /* stack pointer */

	unsigned char delay_timer; /* timers -> goto zero */
	unsigned char sound_timer; /* when zero buzzer is triggered */

	unsigned char key[16]; /* which key are pressed */
	unsigned char gfx[64 * 32]; /* pixel array */

	window_t *window;
} chip8_t;

/*!
 * \brief Initialize a new chip8 structure
 * Malloc a new chip8 structure and initialize all it's internal values / timer
 *
 * \return the new structure
 */
chip8_t *chip8_init(void);

/*!
 * \brief Free a chip8 structure
 *
 * \param chip8 The struct to free
 */
void chip8_free(chip8_t *chip8);

/*!
 * \brief load a game into the chip8_t memory
 * Read as much bytes as possible from the file descriptor and copy it into the memory of the chip8
 * 
 * \param chip8 an initialized chip8
 * \param fd file descriptor where the game belong
 *
 * \return 0 if everything goes well, -1 otherwise
 */
int chip8_load_game(chip8_t *chip8, FILE *file);

/*!
 * \brief Emulate one cycle of the chip8
 * 
 * \param chip8 an initialized chip8 with a loaded game
 *
 * \return 0 if everything goes well, -1 otherwise
 */
int chip8_emulate_cycle(chip8_t *chip8);

#endif /* _VM_H_ */
