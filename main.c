#include "syscall_defs.h"

#include "game.h"
#include "gba.h"

int dbgs = 0, dbge = 0;

void _start() {
    int graphics_res = set_graphics_mode(true);
    if (graphics_res != 0) {
        char fail[] = "Failed to enable graphics mode\n";
        write(0, fail, sizeof(fail));
        exit(1);
    }
    set_tty_mode(TTY_MODE_SCANCODE);
    initVideoBuffer();
    // bool once = false;
    while (1) {
        // input polling
        key_poll();
        if (key_is_down(BUTTON_A)) {
            goto cleanup;
        }

        // game logic
        run();
        presentVideoBuffer();
        wait_ticks(1000 / 60);
    }
cleanup:
    set_tty_mode(TTY_MODE_COOKED);
    set_graphics_mode(false);

    exit(0);
}
