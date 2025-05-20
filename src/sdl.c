#include "sdl.h"

#include <SDL3/SDL.h>
#include <stdio.h>

#include "emulator.h"
#include "log.h"

int emu_sdl_init(Chip8 *emu) {
	SDL_SetAppMetadata("Chip8Emu", "1.0", "me.francesco.chip8");

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		EMU_LOG("SDL_Init()");

		return 1;
	}

	if (!SDL_CreateWindowAndRenderer("Chip8Emu", EMU_WINDOW_WIDTH, EMU_WINDOW_HEIGHT, 0, &emu->window,
									 &emu->renderer)) {
		EMU_LOG("SDL_CreateWindowAndRenderer()");

		return 1;
	}

	return 0;
}

void emu_sdl_handle_key(Chip8 *emu, SDL_Scancode *scancode) {
	switch (*scancode) {
		case SDL_SCANCODE_Q: {
			emu->running = false;
			break;
		}
		case SDL_SCANCODE_ESCAPE: {
			emu->running = false;
			break;
		}
		default: {
			break;
		}
	}
}

void emu_sdl_draw(Chip8 *emu) {
	SDL_RenderClear(emu->renderer);
	SDL_SetRenderDrawColor(emu->renderer, 255, 255, 255, 255);

	int rect_w = EMU_WINDOW_WIDTH / EMU_DISPLAY_WIDTH;
	int rect_h = EMU_WINDOW_HEIGHT / EMU_DISPLAY_HEIGHT;

	for (int y = 0; y < EMU_DISPLAY_HEIGHT; ++y) {
		for (int x = 0; x < EMU_DISPLAY_WIDTH; ++x) {
			if (emu->display[y][x]) {
				SDL_SetRenderDrawColor(emu->renderer, 255, 255, 255, 255);
			} else {
				SDL_SetRenderDrawColor(emu->renderer, 0, 0, 0, 255);
			}

			SDL_FRect rect = {.x = x * rect_w, .y = y * rect_h, .w = rect_w, .h = rect_h};
			SDL_RenderFillRect(emu->renderer, &rect);
		}
	}

	SDL_RenderPresent(emu->renderer);
}

void emu_sdl_free(Chip8 *emu) {
	SDL_DestroyRenderer(emu->renderer);
	SDL_DestroyWindow(emu->window);
	SDL_Quit();
}
