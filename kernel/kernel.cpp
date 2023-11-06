#include <stdint.h>

#include "ps2.h"
#include "keyboard.h"
#include "terminal.h"
#include "printf.h"

extern "C" void kmain() {
    terminal::clear();
    terminal::write_cstr("Los\n");

    ps2::init();
    if (ps2::first.get_type() == 0xab83) {
        ps2::first.enable_scanning();
    } else if (ps2::second.get_type() == 0xab83) {
        ps2::second.enable_scanning();
    } else {
        printf("No keyboard.\n");
        for (;;);
    }

    for (;;) {
        terminal::putchar(keyboard::poll_char());
    }
}
