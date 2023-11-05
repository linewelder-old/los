#include "terminal.h"

extern "C" void kmain() {
    terminal::clear();
    terminal::write_cstr("Hello, world!\n");

    terminal::set_color(terminal::Color::WHITE, terminal::Color::RED);
    for (int i = 0; i < 256; i++) {
        terminal::putchar(static_cast<char>(i));
        if (i % 16 == 15) {
            terminal::putchar('\n');
        }
    }
}
