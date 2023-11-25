#include <kernel/kpanic.h>

#include <stdarg.h>
#include <arch/i386/asm.h>
#include <arch/i386/terminal.h>
#include <kernel/printf.h>

void kpanic(const char* format, ...) {
    va_list args;
    va_start(args, format);

    terminal::set_color(terminal::Color::WHITE, terminal::Color::RED);
    terminal::write_cstr("Kernel panic! ");
    vprintf(format, args);
    disable_interrupts();
    for (;;) hlt();
}
