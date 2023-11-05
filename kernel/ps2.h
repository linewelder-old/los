#pragma once

#include <stdint.h>

namespace ps2 {
    void send(uint8_t data);

    uint8_t poll();

    uint8_t read_config_byte();

    void write_config_byte(uint8_t value);

    void disable_translation();

    void disable_scanning();

    void enable_scanning();
}
