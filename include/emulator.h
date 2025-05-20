#ifndef EMU_H
#define EMU_H

#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define EMU_DISPLAY_WIDTH 64
#define EMU_DISPLAY_HEIGHT 32

#define EMU_MEMORY_SIZE 4096
#define EMU_REGISTERS_NUM 16
#define EMU_STACK_SIZE 64

#define EMU_START_ADDRESS 0x200
#define EMU_FONTSET_START_ADDRESS 0x050
#define EMU_FONTSET_SIZE 80

/* Chip8 struct */
typedef struct chip8_t {
	char memory[EMU_MEMORY_SIZE]; /**< RAM */
	bool display[EMU_DISPLAY_HEIGHT][EMU_DISPLAY_WIDTH];
	uint16_t program_counter;
	uint16_t index_register;
	uint16_t stack[EMU_STACK_SIZE];
	uint8_t delay_timer; /**<  It is decremented at a rate of 60 Hz */
	uint8_t sound_timer; /**< The same as the delay_timer */
	uint8_t registers[EMU_REGISTERS_NUM];
	uint16_t opcode;

	/* SDL3 */
	bool running;
	SDL_Window *window;
	SDL_Renderer *renderer;
} Chip8;

/**
 * Initialize the Chip8 structure
 *
 * @param[out] emu The chip8 structure object
 * @return Returns 0 on success otherwise 1.
 */
int chip8_init(Chip8 *emu);

/**
 * Load fontset into memory
 *
 * @param[in,out] emu The chip8 structure object
 */
void chip8_font_init(Chip8 *emu);

/**
 * Load ROM file into memory
 *
 * @param[in,out] emu The chip8 structure object
 * @param[in] rom Path to the ROM file
 * @return Returns 0 on success otherwise 1
 */
int chip8_load_rom(Chip8 *emu, const char *rom);

/**
 * Fetch next opcode from memory
 *
 * @param[in,out] emu The chip8 structure object
 */
void chip8_fetch(Chip8 *emu);

/**
 * Decode the current opcode and execute the instruction
 *
 * @param[in,out] emu The chip8 structure object
 */
void chip8_decode(Chip8 *emu);

/**
 * Print the entire memory content
 *
 * @param[in] emu The chip8 structure object
 */
void chip8_print_memory(Chip8 *emu);

/**
 * Print the reserved memory content
 *
 * @param[in] emu The chip8 structure object
 */
void chip8_print_reserved_memory(Chip8 *emu);

/**
 * Run the main emulator loop
 *
 * @param[in,out] emu The chip8 structure object
 */
void chip8_run(Chip8 *emu);

/**
 * Free resources used by the emulator
 *
 * @param[in,out] emu The chip8 structure object
 */
void chip8_free(Chip8 *emu);

/* Instructions */

/**
 * Clear the display
 *
 * @param[in,out] emu The chip8 structure object
 */
void chip8_00e0(Chip8 *emu);

/**
 * Jump to address NNN
 *
 * @param[in,out] emu The chip8 structure object
 */
void chip8_1nnn(Chip8 *emu);

/**
 * Set register VX to NN
 *
 * @param[in,out] emu The chip8 structure object
 */
void chip8_6xnn(Chip8 *emu);

/**
 * Add NN to register VX
 *
 * @param[in,out] emu The chip8 structure object
 */
void chip8_7xnn(Chip8 *emu);

/**
 * Set index register I to NNN
 *
 * @param[in,out] emu The chip8 structure object
 */
void chip8_annn(Chip8 *emu);

/**
 * Display N-byte sprite at (VX,VY)
 *
 * @param[in,out] emu The chip8 structure object
 */
void chip8_dxyn(Chip8 *emu);

#endif
