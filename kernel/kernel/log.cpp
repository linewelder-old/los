#include <kernel/log.h>

#include <stdarg.h>

#include <arch/i386/terminal.h>
#include <kernel/printf.h>

void log(const char* severity, const char* file, int line, const char* format, ...) {
    va_list args;
    va_start(args, format);

    printf("%s [%s:%d] ", severity, file, line);
    vprintf(format, args);
    terminal::putchar('\n');

    va_end(args);
}
