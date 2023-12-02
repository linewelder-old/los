#include <arch/i386/ps2.h>

#include <stdint.h>
#include <arch/i386/asm.h>
#include <arch/i386/pic.h>
#include <kernel/log.h>
#include <util/inplace_vector.h>

namespace ps2 {
    static InplaceVector<Device, 2> devices;

    static constexpr uint16_t CONTROL_PORT = 0x64;
    static constexpr uint16_t DATA_PORT = 0x60;

    static const char* get_port_test_fail_reason(uint8_t code) {
        switch (code) {
            case 1: return "clock line stuck low";
            case 2: return "clock line stuck high";
            case 3: return "data line stuck low";
            case 4: return "data line stuck high";
            default: return "reason unkown";
        }
    }

    /// Port: 0 or 1.
    static bool test_port(int port) {
        outb(CONTROL_PORT, port == 0 ? 0xae : 0xa8); // Enable port.
        io_wait();
        outb(CONTROL_PORT, port == 0 ? 0xab : 0xa9); // Perform test.

        uint8_t response = 0;
        if (!try_poll(response)) {
            LOG_ERROR("PS/2 port %d test failed, no response.",
                port);
            return false;
        } else if (response != 0x00) {
            LOG_ERROR("PS/2 port %d test failed, %s (code 0x%x).",
                port, get_port_test_fail_reason(response), response);
            return false;
        }

        return true;
    }

    /// Port: 0 or 1.
    static void init_device(int port) {
        outb(CONTROL_PORT, port == 0 ? 0xae : 0xa8); // Enable port.
        io_wait();
        Device(port).send(0xff); // Reset.

        uint8_t response = 0;
        if (!try_poll(response)) {
            LOG_WARN("PS/2 device %d reset failed, no response.",
                port);
            return;
        } else if (response != 0xfa) {
            LOG_ERROR("PS/2 device %d reset failed, received 0x%x instead of 0xfa.",
                port, response);
            return;
        }

        if (!try_poll(response)) {
            LOG_ERROR("PS/2 device %d reset failed, no status code.",
                port);
            return;
        } else if (response != 0xaa) {
            LOG_ERROR("PS/2 device %d reset failed, status code 0x%x instead of 0xaa.",
                port, response);
            return;
        }

        // A mouse sends its device type after test.
        try_poll(response);

        devices.push_back(Device(port));
        Device& device = devices[devices.get_count() - 1];

        device.disable_scanning();
        device.identify();
        write_config_byte(read_config_byte() | (1 << port)); // Enable interrupts for the device.
    }

    void init() {
        outb(CONTROL_PORT, 0xad); // Disable port 0.
        io_wait();
        outb(CONTROL_PORT, 0xa7); // Disbale port 1.
        io_wait();

        inb(DATA_PORT); // Flush the output buffer.

        uint8_t config_byte = read_config_byte();
        config_byte &= ~(1 << 0); // Disable port 0 interrupt.
        config_byte &= ~(1 << 1); // Disable port 1 interrupt.
        config_byte &= ~(1 << 6); // Disable translation.
        write_config_byte(config_byte);

        outb(CONTROL_PORT, 0xaa); // Perform self test.
        uint8_t response = 0;
        if (!try_poll(response)) {
            LOG_ERROR("PS/2 controller self test failed, no response");
            return;
        } else if (response != 0x55) {
            LOG_ERROR("PS/2 controller self test failed, received 0x%x instead of 0x55",
                response);
            return;
        }

        // On some devices the controller resets after the test.
        // Restore the configuration byte.
        write_config_byte(config_byte);

        bool two_channels = true;

        // Determine if there are 2 channels.
        outb(CONTROL_PORT, 0xa8); // Try enabling device 1.
        io_wait();
        if (read_config_byte() & (1 << 5)) {
            LOG_INFO("PS/2 controller has one channel.");
            two_channels = false;
        } else {
            outb(CONTROL_PORT, 0xa7); // Disable device 1.
        }

        bool port_0_available = test_port(0);
        bool port_1_available = two_channels && test_port(1);

        if (!port_0_available && !port_1_available) {
            LOG_WARN("No PS/2 ports available.");
            return;
        }

        if (port_0_available) {
            init_device(0);
        }

        if (port_1_available) {
            init_device(1);
        }
    }

    const Device& get_device(size_t id) {
        return devices[id];
    }

    size_t get_device_count() {
        return devices.get_count();
    }

    Option<const Device&> find_device_with_type(uint16_t type) {
        for (size_t i = 0; i < devices.get_count(); i++) {
            if (devices[i].get_type() == type) {
                return devices[i];
            }
        }
        return {};
    }

    void Device::send(uint8_t data) const {
        if (this->id == 1) {
            outb(CONTROL_PORT, 0xd4);
        }
        while (inb(CONTROL_PORT) & 2);
        outb(DATA_PORT, data);
    }

    static bool expect_ack(const char* what, int device_id) {
        uint8_t response = 0;
        if (!try_poll(response)) {
            LOG_ERROR("%s (PS/2 device %d) failed, didn't receive device response",
                what, device_id);
            return false;
        }

        if (response != 0xfa) {
            LOG_ERROR("%s (PS/2 device %d) failed, received 0x%x instead of 0xfa",
                what, device_id, response);
            return false;
        }

        return true;
    }

    void Device::disable_scanning() const {
        send(0xf5);
        expect_ack("Disable scanning", id);
    }

    void Device::enable_scanning() const {
        send(0xf4);
        expect_ack("Enable scanning", id);
    }

    void Device::identify() {
        send(0xf2);
        expect_ack("Identify", id);

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

    void Device::set_interrupt_handler(idt::Handler handler) const {
        idt::register_interrupt(
            pic::get_interrupt_vector(id == 0 ? 1 : 12), handler);
    }

    bool try_poll(uint8_t& output, int max_cycles) {
        int cycles = 0;
        while (!(inb(CONTROL_PORT) & 1) && cycles < max_cycles) {
            tiny_delay();
            cycles++;
            if (cycles == max_cycles) return false;
        }

        output = inb(DATA_PORT);
        return true;
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
}
