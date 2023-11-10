#include <stdint.h>

#include "asm.h"
#include "gdt.h"
#include "idt.h"
#include "exceptions.h"
#include "pic.h"
#include "ps2.h"
#include "keyboard.h"
#include "terminal.h"
#include "printf.h"

extern "C" void kmain() {
    disable_interrupts();
    gdt::init();

    idt::init();
    register_exception_handlers();
    pic::init(0x20, 0x28);

    terminal::clear();
    terminal::write_cstr("Los\n");

    ps2::init();
    bool keyboard_found = false;
    terminal::write_cstr("Connected PS/2 devices:\n");
    for (int i = 0; i < ps2::get_device_count(); i++) {
        const ps2::Device& device = ps2::get_device(i);
        printf("%d: %s (type: %x)",
            i, device.get_type_name(), device.get_type());

        if (device.get_type() == 0xab83) {
            device.enable_scanning();
            idt::register_interrupt(i == 0 ? 
                0x21 : 0x2c, keyboard::irq_handler);
            keyboard_found = true;
            terminal::write_cstr(" [Primary keyboard]");
        }

        terminal::putchar('\n');
    }
    if (!keyboard_found) kpanic("No keyboard");

    terminal::putchar('\n');

    keyboard::set_callback([](char ch) {
        terminal::putchar(ch);
    });
    pic::clear_mask(1);
    enable_interrupts();
    for (;;) {
        asm volatile("hlt");
    }
}
