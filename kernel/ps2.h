#pragma once

#include <stdint.h>

namespace ps2 {
    class Device {
    public:
        constexpr Device(uint16_t id) : id(id) {}

        void send(uint8_t data);
        void disable_scanning();
        void enable_scanning();

    private:
        uint16_t id; /* 0 or 1 */
    };

    extern class Device first;
    extern class Device second;

    uint8_t poll();

    uint8_t read_config_byte();

    void write_config_byte(uint8_t value);

    void disable_translation();
}
