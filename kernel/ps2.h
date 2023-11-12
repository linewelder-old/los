#pragma once

#include <stdint.h>
#include "idt.h"

namespace ps2 {
    class Device {
    public:
        constexpr Device() : id(0), type(0xffff) {}
        constexpr Device(uint16_t id) : id(id), type(0xffff) {}

        void send(uint8_t data) const;
        void disable_scanning() const;
        void enable_scanning() const;

        uint16_t get_type() const;
        const char* get_type_name() const;
        void identify();

        void set_interrupt_handler(idt::Handler handler) const;

    private:
        uint16_t id; /* 0 or 1 */
        uint16_t type; /** 0xffff if the device returns an empty response. */
    };

    void init();

    const Device& get_device(int id);

    /// 1 or 2.
    int get_device_count();

    bool try_poll(uint8_t& output, int max_cycles = 500);

    uint8_t read_input();

    uint8_t read_config_byte();

    void write_config_byte(uint8_t value);
}
