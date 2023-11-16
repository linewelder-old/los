#include "pci.h"

#include "asm.h"

namespace pci {
    static constexpr uint16_t CONFIG_ADDRESS_PORT = 0xcf8;
    static constexpr uint16_t CONFIG_DATA_PORT = 0xcfc;

    /**
     * bus      - 8 bits available (up to 0xff).
     * device   - 5 bits available (up to 0x20).
     * function - 3 bits available (up to 0x08).
     * offset   - 8 bits available (up to 0xff),
     *            has to be aligned to 4 bytes.
     */
    uint32_t config_read_u32(
        uint8_t bus, uint8_t device,
        uint8_t function, uint8_t offset)
    {
        uint32_t address =
            0x80000000 |
            (static_cast<uint32_t>(bus) << 16) |
            (static_cast<uint32_t>(device << 11)) |
            (static_cast<uint32_t>(function << 8)) |
            static_cast<uint32_t>(offset);

        outl(CONFIG_ADDRESS_PORT, address);
        return inl(CONFIG_DATA_PORT);
    }

    /**
     * bus      - 8 bits available (up to 0xff).
     * device   - 5 bits available (up to 0x20).
     * function - 3 bits available (up to 0x08).
     * offset   - 8 bits available (up to 0xff),
     *            has to be aligned to 2 bytes.
     */
    uint16_t config_read_u16(
        uint8_t bus, uint8_t device,
        uint8_t function, uint8_t offset)
    {
        uint32_t dword = config_read_u32(bus, device, function, offset & 0xfc);
        if ((offset & 0x03) == 0) {
            return dword & 0xffff;
        } else {
            return dword >> 16;
        }
    }

    /**
     * bus      - 8 bits available (up to 0xff).
     * device   - 5 bits available (up to 0x20).
     * function - 3 bits available (up to 0x08).
     * offset   - 8 bits available (up to 0xff).
     */
    uint16_t config_read_u8(
        uint8_t bus, uint8_t device,
        uint8_t function, uint8_t offset)
    {
        uint32_t dword = config_read_u32(bus, device, function, offset & 0xfc);
        uint8_t offset_within_dword = (offset & 0x03) * 8;
        return (dword >> offset_within_dword) & 0xff;
    }
}
