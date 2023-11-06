#include <stdint.h>

#include "ps2.h"
#include "keyboard.h"
#include "terminal.h"

extern "C" void kmain() {
    terminal::clear();
    terminal::write_cstr("Los\n");

    ps2::disable_translation();
    ps2::first.enable_scanning();

    for (;;) {
        terminal::putchar(keyboard::poll_char());
    }
}
