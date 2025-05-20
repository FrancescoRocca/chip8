#include <stdio.h>
#include <stdlib.h>

#include "emulator.h"
#include "log.h"

int main(int argc, char **argv) {
	if (argc < 2) {
		EMU_ERROR("Please provide at least one chip8 program");
		exit(EXIT_FAILURE);
	}

	int ret;

	EMU_INFO("Press Q or ESC to quit.");

	/* Init emulator object */
	Chip8 emu;
	ret = chip8_init(&emu);
	if (ret) {
		EMU_ERROR("Unable to init Chip8");

		return 1;
	}

	/* Open rom */
	EMU_INFO("Opening %s", argv[1]);
	ret = chip8_load_rom(&emu, argv[1]);

#ifdef EVELOPER
	chip8_print_memory(&emu);
#endif

	if (ret) {
		EMU_ERROR("Unable to open %s", argv[1]);

		return 1;
	}

	chip8_run(&emu);

	EMU_INFO("Closing...");

	chip8_free(&emu);

	return 0;
}
