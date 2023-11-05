#include "terminal.h"

extern "C" void kmain() {
    terminal::clear();
    terminal::write_cstr("Hello, world!");

    for (int i = 0; i < 256; i++) {
        terminal::put_entry_at(
            static_cast<char>(i),
            terminal::Color::WHITE, terminal::Color::RED,
            i % 16, i / 16 + 1);
    }
}
