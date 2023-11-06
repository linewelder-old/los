#include "ps2.h"

#include <stdint.h>
#include "asm.h"

namespace ps2 {
    Device first(0);
    Device second(1);

    static constexpr uint16_t CONTROL_PORT = 0x64;
    static constexpr uint16_t DATA_PORT = 0x60;

    void Device::send(uint8_t data) {
        if (this->id == 1) {
            outb(CONTROL_PORT, 0xd4);
        }
        while (inb(CONTROL_PORT) & 2);
        outb(DATA_PORT, data);
    }

    void Device::disable_scanning() {
        send(0xf5);
        while (poll() != 0xfa);
    }

    void Device::enable_scanning() {
        send(0xf4);
        while (poll() != 0xfa);
    }

    uint8_t poll() {
        while (!(inb(CONTROL_PORT) & 1));
        return (uint8_t)inb(DATA_PORT);
    }

    uint8_t read_config_byte() {
        outb(CONTROL_PORT, 0x20);
        return inb(DATA_PORT);
    }

    void write_config_byte(uint8_t value) {
        outb(CONTROL_PORT, 0x60);
        outb(DATA_PORT, value);
    }

    void disable_translation() {
        write_config_byte(read_config_byte() ^ 64);
    }
}
