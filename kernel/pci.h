#pragma once

#include <stdint.h>
#include <stddef.h>

namespace pci {
    class Function {
    public:
        constexpr Function() : bus(0), device(0), function(0) {}
        constexpr Function(uint8_t bus, uint8_t device, uint8_t function)
            : bus(bus), device(device), function(function) {}

        uint8_t get_bus() const;
        uint8_t get_device() const;
        uint8_t get_function() const;

        uint16_t get_vendor() const;
        uint16_t get_device_id() const;
        uint16_t get_full_class() const;
        bool has_multiple_functions() const;

        /// For PCI-to-PCI bridges only.
        uint8_t get_secondary_bus() const;

    private:
        uint8_t bus;
        uint8_t device;
        uint8_t function;
    };

    const Function& get_function(size_t index);

    size_t get_function_count();

    void init();
}
