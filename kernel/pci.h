#pragma once

#include <stdint.h>

namespace pci {
    /**
     * bus      - 8 bits available (up to 0xff).
     * device   - 5 bits available (up to 0x20).
     * function - 3 bits available (up to 0x08).
     * offset   - 8 bits available (up to 0xff),
     *            has to be aligned to 4 bytes.
     */
    uint32_t config_read_u32(
        uint8_t bus, uint8_t device,
        uint8_t function, uint8_t offset);

    /**
     * bus      - 8 bits available (up to 0xff).
     * device   - 5 bits available (up to 0x20).
     * function - 3 bits available (up to 0x08).
     * offset   - 8 bits available (up to 0xff),
     *            has to be aligned to 2 bytes.
     */
    uint16_t config_read_u16(
        uint8_t bus, uint8_t device,
        uint8_t function, uint8_t offset);

    /**
     * bus      - 8 bits available (up to 0xff).
     * device   - 5 bits available (up to 0x20).
     * function - 3 bits available (up to 0x08).
     * offset   - 8 bits available (up to 0xff).
     */
    uint16_t config_read_u8(
        uint8_t bus, uint8_t device,
        uint8_t function, uint8_t offset);
}
