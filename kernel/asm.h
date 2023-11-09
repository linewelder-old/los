#pragma once

#include <stdint.h>

inline void outl(uint16_t address, uint32_t data) {
    asm volatile("outl %1, %0"
        :
        : "d" (address), "a" (data));
}

inline void outb(uint16_t address, uint8_t data) {
    asm volatile("outb %1, %0"
        :
        : "d" (address), "a" (data));
}

inline uint32_t inl(uint16_t address) {
    uint32_t result;
    asm volatile("inl %1, %0"
        : "=a" (result)
        : "d" (address));
    return result;
}

inline uint8_t inb(uint16_t address) {
    uint8_t result;
    asm volatile("inb %1, %0"
        : "=a" (result)
        : "d" (address));
    return result;
}

inline void cli() {
    asm volatile("cli");
}
