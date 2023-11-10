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
    printf("Connected PS/2 devices:\n");
    for (int i = 0; i < ps2::get_device_count(); i++) {
        const ps2::Device& device = ps2::get_device(i);
        printf("%d: %s (%x)\n",
            i, device.get_type_name(), device.get_type());
    }

    bool keyboard_found = false;
    for (int i = 0; i < ps2::get_device_count(); i++) {
        const ps2::Device& device = ps2::get_device(i);
        if (device.get_type() == 0xab83) {
            device.enable_scanning();
            keyboard_found = true;
        }
    }
    if (!keyboard_found) kpanic("No keyboard");

}
