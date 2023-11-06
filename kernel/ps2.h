#pragma once

#include <stdint.h>

namespace ps2 {
    class Device {
    public:
        constexpr Device(uint16_t id) : id(id), type(0xffff) {}

        void send(uint8_t data);
        void disable_scanning();
        void enable_scanning();

        uint16_t get_type();
        const char* get_type_name();
        void identify();

    private:
        uint16_t id; /* 0 or 1 */
        uint16_t type; /** 0xffff if the device returns an empty response. */
    };

    extern class Device first;
    extern class Device second;

    void init();

    bool try_poll(uint8_t& output, int max_cycles = 20);

    uint8_t poll();

    uint8_t read_config_byte();

    void write_config_byte(uint8_t value);

    void disable_translation();
}
