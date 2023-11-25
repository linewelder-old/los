#pragma once

/// See: printf.
void kpanic(const char* message, ...);

void register_exception_handlers();
