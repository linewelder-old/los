#include <kernel/log.h>

#include <stdarg.h>

#include <arch/i386/terminal.h>
#include <kernel/printf.h>

namespace log {
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

        printf("%s [%s:%d] ", get_severity_str(severity), file, line);
        vprintf(format, args);
        terminal::putchar('\n');

        va_end(args);
    }
}
