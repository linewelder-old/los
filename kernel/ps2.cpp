#include "ps2.h"

#include <stdint.h>
#include "asm.h"

namespace ps2 {
    static Device devices[] = { Device(0), Device(1) };

    static constexpr uint16_t CONTROL_PORT = 0x64;
    static constexpr uint16_t DATA_PORT = 0x60;

    void init() {
        disable_translation();

        for (int i = 0; i < get_device_count(); i++) {
            devices[i].disable_scanning();
        }

        for (int i = 0; i < get_device_count(); i++) {
            devices[i].identify();
        }
    }

    const Device& get_device(int id) {
        return devices[id];
    }

    int get_device_count() {
        return 2; // For now we assume both devices are available.
    }

    void Device::send(uint8_t data) const {
        if (this->id == 1) {
            outb(CONTROL_PORT, 0xd4);
        }
        while (inb(CONTROL_PORT) & 2);
        outb(DATA_PORT, data);
    }

    void Device::disable_scanning() const {
        send(0xf5);
        while (poll() != 0xfa);
    }

    void Device::enable_scanning() const {
        send(0xf4);
        while (poll() != 0xfa);
    }

    void Device::identify() {
        send(0xf2);
        while (poll() != 0xfa);

        uint16_t response = 0;
        for (int i = 0; i < 2; i++) {
            uint8_t buf;
            if (try_poll(buf)) {
                response = response << 8 | buf;
            } else if (i == 0) {
                response = 0xffff;
            } else {
                break;
            }
        }

        type = response;
    }

    uint16_t Device::get_type() const {
        return type;
    }

    const char* Device::get_type_name() const {
        switch (type) {
            case 0x00: return "Standard PS/2 mouse";
            case 0x03: return "Mouse with scroll wheel";
            case 0x04: return "5-button mouse";
            case 0xab83:
            case 0xabc1: return "MF2 keyboard";
            case 0xab41: return "MF2 keyboard (translated)";
            case 0xab84: return "\"Short\" keyboard";
            case 0xab54: return "\"Short\" keyboard (translated)";
            case 0xab85: return "NCD N-97 keyboard";
            case 0xab86: return "122-key keyboard";
            case 0xab90: return "Japanese \"G\" keyboard";
            case 0xab91: return "Japanese \"P\" keyboard";
            case 0xab92: return "Japanese \"A\" keyboard";
            case 0xaba1: return "NCD Sun layout keyboard";
            case 0xffff: return "Ancient AT keyboard";
            default: return "Unknown";
        }
    }

    bool try_poll(uint8_t& output, int max_cycles) {
        int cycles = 0;
        while (!(inb(CONTROL_PORT) & 1) && cycles < max_cycles) {
            cycles++;
            if (cycles == max_cycles) return false;
        }

        output = inb(DATA_PORT);
        return true;
    }

    uint8_t poll() {
        while (!(inb(CONTROL_PORT) & 1));
        return (uint8_t)inb(DATA_PORT);
    }

    uint8_t read_input() {
        return inb(DATA_PORT);
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
        write_config_byte(read_config_byte() & ~64);
    }
}
