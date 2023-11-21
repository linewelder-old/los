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
#include "pci.h"
#include "ide.h"

extern "C" void kmain() {
    terminal::clear();
    terminal::write_cstr("Los\n");

    gdt::init();
    idt::init();
    register_exception_handlers();

    terminal::write_cstr("Initializing PIC...\n");
    pic::init();

    terminal::write_cstr("Initializing the PS/2 controller...\n");
    ps2::init();

    bool keyboard_found = false;
    terminal::write_cstr("\nConnected PS/2 devices:\n");
    for (int i = 0; i < ps2::get_device_count(); i++) {
        const ps2::Device& device = ps2::get_device(i);
        printf("- %s (type: %x)",
            device.get_type_name(), device.get_type());

        if (!keyboard_found && device.get_type() == 0xab83) {
            device.enable_scanning();
            device.set_interrupt_handler(keyboard::irq_handler);
            keyboard_found = true;
            terminal::write_cstr(" [Primary keyboard]");
        }

        terminal::putchar('\n');
    }
    if (!keyboard_found) kpanic("No keyboard");

    terminal::write_cstr("\nDetecting connected PCI devices...\n");
    pci::init();

    terminal::write_cstr("\nConnected PCI devices:\n");
    for (size_t i = 0; i < pci::get_function_count(); i++) {
        const pci::Function& func = pci::get_function(i);
        printf(
            "%d:%d.%d Class: %x Vendor: %x Device: %x\n",
            func.get_bus(), func.get_device(), func.get_function(),
            func.get_full_class(),
            func.get_vendor(), func.get_device_id());

        // PCI IDE
        if (func.get_full_class() == 0x0101) {
            printf("  Prog IF: %x\n", func.get_prog_if());
            for (int i = 0; i < 5; i++) {
                printf("  BAR%d: %x  ", i, func.get_bar_io(i));
            }
            terminal::putchar('\n');
            ide::init();
        }
    }

    keyboard::set_callback([](keyboard::KeyEventArgs args) {
        if (!args.released && args.character) {
            terminal::putchar(args.character);
        }
    });
    pic::clear_mask(1);
    enable_interrupts();

    terminal::write_cstr("\nYou can type\n\n");
    for (;;) {
        asm volatile("hlt");
    }
}
