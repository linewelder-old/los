#include "terminal.h"

#include <stdint.h>
#include <stddef.h>

namespace terminal {
    static uint16_t* const BUFFER = (uint16_t*)0xb8000;

    static constexpr inline uint8_t vga_entry_color(Color fg, Color bg) {
        return static_cast<uint8_t>(fg) | static_cast<uint8_t>(bg) << 4;
    }

    static constexpr inline uint16_t vga_entry(unsigned char ch, uint8_t color) {
        return static_cast<uint16_t>(ch) | static_cast<uint16_t>(color) << 8;
    }

    static size_t row = 0;
    static size_t column = 0;
    static uint8_t color = vga_entry_color(Color::LIGHT_GREY, Color::BLACK);

    void set_color(Color fg, Color bg) {
        color = vga_entry_color(fg, bg);
    }

    void put_entry_at(char ch, Color fg, Color bg, size_t x, size_t y) {
        const size_t index = y * WIDTH + x;
        BUFFER[index] = vga_entry(ch, vga_entry_color(fg, bg));
    }

    void clear() {
        row = 0;
        column = 0;
        for (size_t y = 0; y < HEIGHT; y++) {
            for (size_t x = 0; x < WIDTH; x++) {
                put_entry_at(' ', terminal::Color::LIGHT_GREY, terminal::Color::BLACK, x, y);
            }
        }
    }

    void putchar(char ch) {
        const size_t index = row * WIDTH + column;
        BUFFER[index] = vga_entry(ch, color);

        column++;
        if (column == WIDTH) {
            column = 0;
            row++;
            if (row == HEIGHT) {
                row = 0;
            }
        }
    }

    void write(const char* str, size_t len) {
        for (size_t i = 0; i < len; i++) {
            putchar(str[i]);
        }
    }

    void write_cstr(const char* str) {
        while (str[0]) {
            putchar(str[0]);
            str++;
        }
    }
}
