#ifndef EMU_SDL_H
#define EMU_SDL_H

#include "emulator.h"

#define EMU_WINDOW_WIDTH 640
#define EMU_WINDOW_HEIGHT 480

int emu_sdl_init(Chip8 *emu);

void emu_sdl_handle_key(Chip8 *emu, SDL_Scancode *scancode);

void emu_sdl_draw(Chip8 *emu);

void emu_sdl_free(Chip8 *emu);

#endif
