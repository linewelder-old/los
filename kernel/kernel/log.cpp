#include <kernel/log.h>

#include <stdarg.h>

#include <arch/i386/terminal.h>
#include <kernel/printf.h>

namespace log {
    struct SeverityStyle {
        const char* name;
        terminal::Color color;
    };

    static SeverityStyle get_severity_style(SeverityLevel level) {
        switch (level) {
            case SeverityLevel::INFO:    return { "INF", terminal::Color::DARK_GREY };
            case SeverityLevel::WARNING: return { "WRN", terminal::Color::LIGHT_BROWN };
            case SeverityLevel::ERROR:   return { "ERR", terminal::Color::LIGHT_RED };
            default: __builtin_unreachable();
        }
    }

    void log(SeverityLevel severity, const char* file, int line, const char* format, ...) {
        va_list args;
        va_start(args, format);

        SeverityStyle style = get_severity_style(severity);
        terminal::set_color(style.color, terminal::Color::BLACK);

        printf("%s [%s:%d] ", style.name, file, line);
        vprintf(format, args);
        terminal::putchar('\n');
        terminal::set_color(terminal::Color::LIGHT_GREY, terminal::Color::BLACK);

        va_end(args);
    }
}
