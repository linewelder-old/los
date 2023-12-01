#include <stdint.h>

#include <arch/i386/asm.h>
#include <arch/i386/gdt.h>
#include <arch/i386/idt.h>
#include <arch/i386/exceptions.h>
#include <arch/i386/pic.h>
#include <arch/i386/ps2.h>
#include <arch/i386/keyboard.h>
#include <arch/i386/terminal.h>
#include <arch/i386/pci.h>
#include <arch/i386/ide.h>
#include <kernel/log.h>
#include <kernel/printf.h>
#include <kernel/kpanic.h>
#include <kernel/multiboot.h>

extern "C"
void kmain(multiboot_info_t* multiboot_info, uint32_t magic) {
    terminal::clear();
    terminal::write_cstr("Los\n");

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        kpanic("Invalid Multiboot magic number");
    }

    gdt::init();
    idt::init();
    register_exception_handlers();

    LOG_INFO("Initializing PIC...");
    pic::init();

    LOG_INFO("Initializing the PS/2 controller...");
    ps2::init();

    terminal::write_cstr("\nConnected PS/2 devices:\n");
    for (size_t i = 0; i < ps2::get_device_count(); i++) {
        const ps2::Device& device = ps2::get_device(i);
        printf("- %s (type: %x)\n",
            device.get_type_name(), device.get_type());
    }

    Option<const ps2::Device&> keyboard = ps2::find_device_with_type(0xab83);
    if (keyboard.has_value()) {
        keyboard->enable_scanning();
        keyboard->set_interrupt_handler(keyboard::irq_handler);
    } else {
        LOG_ERROR("No keyboard");
    }

    LOG_INFO("Detecting connected PCI devices...");
    pci::init();

    terminal::write_cstr("\nConnected PCI devices:\n");
    for (size_t i = 0; i < pci::get_function_count(); i++) {
        const pci::Function& func = pci::get_function(i);
        printf(
            "%d:%d.%d Class: %x Vendor: %x Device: %x\n",
            func.get_bus(), func.get_device(), func.get_function(),
            func.get_full_class(),
            func.get_vendor(), func.get_device_id());
    }

    Option<const pci::Function&> ide_controller =
        pci::find_function_with_class(0x0101);
    if (ide_controller.has_value()) {
        ide::init(ide_controller.get_value());
    } else {
        LOG_ERROR("No IDE controller");
    }

    terminal::write_cstr("\nConnected disks:\n");
    for (size_t i = 0; i < ide::get_disk_count(); i++) {
        const ide::Device& disk = ide::get_disk(i);
        printf("- %s (%d Kb) Inteface: %s\n",
            disk.model, disk.size / 2,
            (const char*[]){ "ATA", "ATAPI" }[(int)disk.interface]);
    };

    keyboard::set_callback([](keyboard::KeyEventArgs args) {
        if (!args.released && args.character) {
            terminal::putchar(args.character);
        }
    });
    pic::clear_mask(1);
    enable_interrupts();

    terminal::write_cstr("\nYou can type\n\n");
    for (;;) hlt();
}
