#include "emulator.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "log.h"
#include "sdl.h"

int chip8_init(Chip8 *emu) {
	srand(time(NULL));

	memset(emu->memory, 0, sizeof(emu->memory));
	memset(emu->display, 0, sizeof(emu->display));
	memset(emu->stack, 0, sizeof(emu->stack));
	memset(emu->registers, 0, sizeof(emu->registers));

	int ret = emu_sdl_init(emu);
	if (ret) {
		return 1;
	}

	chip8_font_init(emu);

	emu->program_counter = EMU_START_ADDRESS;
	emu->index_register = 0;
	emu->running = true;

	return 0;
}

void chip8_font_init(Chip8 *emu) {
	char fontset[EMU_FONTSET_SIZE] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
		0x20, 0x60, 0x20, 0x20, 0x70,  // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
		0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
		0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
		0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
		0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
		0xF0, 0x80, 0xF0, 0x80, 0x80   // F
	};

	/* Load the fontset in memory */
	for (int i = 0; i < EMU_FONTSET_SIZE; ++i) {
		emu->memory[EMU_FONTSET_START_ADDRESS + i] = fontset[i];
	}
}

int chip8_load_rom(Chip8 *emu, const char *rom) {
	if (rom == NULL || emu == NULL) {
		EMU_ERROR("ROM path or emulator instance is NULL.");

		return 1;
	}

	FILE *fp = fopen(rom, "rb");
	if (fp == NULL) {
		EMU_ERROR("Failed to open ROM file '%s': %s", rom, strerror(errno));

		return 1;
	}

	/* Get ROM size */
	if (fseek(fp, 0, SEEK_END) != 0) {
		EMU_ERROR("fseek() failed: %s", strerror(errno));
		fclose(fp);

		return 1;
	}

	long rom_content_len = ftell(fp);
	if (rom_content_len < 0) {
		EMU_ERROR("ftell() failed: %s", strerror(errno));
		fclose(fp);

		return 1;
	}

	rewind(fp);

	/* Read ROM into memory */
	size_t ret = fread(emu->memory + EMU_START_ADDRESS, 1, rom_content_len, fp);
	if (ret != (size_t)rom_content_len) {
		EMU_ERROR("Failed to read ROM file: %s", strerror(errno));
		fclose(fp);

		return 1;
	}

	fclose(fp);
	return 0;
}

void chip8_run(Chip8 *emu) {
	SDL_Event event;
	uint32_t last_time = SDL_GetTicks();

	while (emu->running) {
		uint32_t current_time = SDL_GetTicks();
		/* Max of 500 instructions */
		if (current_time - last_time > 2) {
			chip8_fetch(emu);
			chip8_decode(emu);
			last_time = current_time;
		}

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_KEY_DOWN) {
				emu_sdl_handle_key(emu, &event.key.scancode);
				continue;
			}

			switch (event.type) {
				case SDL_EVENT_QUIT: {
					emu->running = false;
					break;
				}
			}
		}

		SDL_Delay(1);
	}

	chip8_free(emu);
}

void chip8_fetch(Chip8 *emu) {
	if (emu->program_counter >= EMU_MEMORY_SIZE) {
		return;
	}

	int16_t op1 = (unsigned char)emu->memory[emu->program_counter++];
	int16_t op2 = (unsigned char)emu->memory[emu->program_counter++];
	op1 <<= 8;
	emu->opcode = op1 | op2;
	if (emu->opcode == 0x0) {
		exit(1);
	}
}

void chip8_decode(Chip8 *emu) {
	switch (emu->opcode & 0xF000) {
		case 0x0000: {
			switch (emu->opcode & 0x00FF) {
				case 0x00E0: {
					chip8_00e0(emu);
					break;
				}
			}
			break;
		}
		case 0x1000: {
			chip8_1nnn(emu);
			break;
		}
		case 0x6000: {
			chip8_6xnn(emu);
			break;
		}
		case 0x7000: {
			chip8_7xnn(emu);
			break;
		}
		case 0xA000: {
			chip8_annn(emu);
			break;
		}
		case 0xD000: {
			chip8_dxyn(emu);
			break;
		}
		default: {
			EMU_ERROR("Invalid opcode: %#x", emu->opcode);
		}
	}
}

void chip8_print_memory(Chip8 *emu) {
	EMU_LOG("Memory content:");
	for (int i = 0; i < EMU_MEMORY_SIZE; ++i) {
		/* I need to use a mask because int is bigger than char */
		fprintf(stdout, "%02x ", (unsigned char)emu->memory[i]);
	}
	fprintf(stdout, "\n");
}

void chip8_print_reserved_memory(Chip8 *emu) {
	EMU_LOG("Reserved memory content:");
	for (int i = 0; i < EMU_START_ADDRESS; ++i) {
		fprintf(stdout, "%02x ", (unsigned char)emu->memory[i]);
	}
	fprintf(stdout, "\n");
}

void chip8_free(Chip8 *emu) { emu_sdl_free(emu); }

void chip8_00e0(Chip8 *emu) {
	/* Clear screen */
	memset(emu->display, 0, sizeof(emu->display));
	emu_sdl_draw(emu);
}

void chip8_1nnn(Chip8 *emu) {
	/* Jump */
	emu->program_counter = emu->opcode & 0x0FFF;
}

void chip8_6xnn(Chip8 *emu) {
	/* Set register VX */
	uint16_t reg = (emu->opcode & 0x0F00) >> 8;
	uint16_t value = emu->opcode & 0x00FF;
	emu->registers[reg] = value;
}

void chip8_7xnn(Chip8 *emu) {
	/* Add value to register VX */
	uint16_t reg = (emu->opcode & 0x0F00) >> 8;
	uint16_t value = emu->opcode & 0x00FF;
	emu->registers[reg] += value;
}

void chip8_annn(Chip8 *emu) {
	/* Set index register I */
	int16_t value = emu->opcode & 0x0FFF;
	emu->index_register = value;
}

void chip8_dxyn(Chip8 *emu) {
	/* Draw */
	uint16_t opcode = emu->opcode;
	int vx = (opcode & 0x0F00) >> 8;
	int vy = (opcode & 0x00F0) >> 4;
	int x = emu->registers[vx] % EMU_DISPLAY_WIDTH;
	int y = emu->registers[vy] % EMU_DISPLAY_HEIGHT;
	int rows = opcode & 0x000F;

	emu->registers[0xF] = 0;

	for (int row = 0; row < rows; ++row) {
		uint8_t sprite_data = emu->memory[emu->index_register + row];
		for (int col = 0; col < 8; ++col) {
			uint8_t pixel = (sprite_data & (0x80 >> col)) != 0;

			if (!pixel) continue;

			int xx = (x + col) % EMU_DISPLAY_WIDTH;
			int yy = (y + row) % EMU_DISPLAY_HEIGHT;

			if (emu->display[yy][xx]) {
				emu->registers[0xF] = 1;
			}
			emu->display[yy][xx] ^= 1;
		}
	}

	emu_sdl_draw(emu);
}
