#include <stdint.h>

#include "asm.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "ps2.h"
#include "keyboard.h"
#include "terminal.h"
#include "printf.h"

extern "C" void kmain() {
    disable_interrupts();
    gdt::init();

    idt::init();
    pic::init(0x20, 0x28);

    terminal::clear();
    terminal::write_cstr("Los\n");

    ps2::init();
    printf("Connected PS/2 devices:\n");
    printf("0: %s (%x)\n",
        ps2::first.get_type_name(), ps2::first.get_type());
    printf("1: %s (%x)\n",
        ps2::second.get_type_name(), ps2::second.get_type());

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
