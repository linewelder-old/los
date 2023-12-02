#include <kernel/log.h>

#include <stdarg.h>

#include <arch/i386/terminal.h>
#include <kernel/printf.h>

static const char* get_severity_str(SeverityLevel level) {
    switch (level) {
        case SeverityLevel::INFO:    return "INF";
        case SeverityLevel::WARNING: return "WRN";
        case SeverityLevel::ERROR:   return "ERR";
        default: __builtin_unreachable();
    }
}

void log(SeverityLevel severity, const char* file, int line, const char* format, ...) {
    va_list args;
    va_start(args, format);

    terminal::set_color(terminal::Color::DARK_GREY, terminal::Color::BLACK);
    printf("%s [%s:%d] ", get_severity_str(severity), file, line);
    vprintf(format, args);
    terminal::putchar('\n');
    terminal::set_color(terminal::Color::LIGHT_GREY, terminal::Color::BLACK);

    va_end(args);
}
