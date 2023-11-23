#include "ide.h"

#include "asm.h"
#include "printf.h"

namespace ide {
    // Flags in the Status register.
    constexpr uint8_t STATUS_BUSY                = 0x80;
    constexpr uint8_t STATUS_DRIVE_READY         = 0x40;
    constexpr uint8_t STATUS_DRIVE_WRITE_FAULT   = 0x20;
    constexpr uint8_t STATUS_DRIVE_SEEK_COMPLETE = 0x10;
    constexpr uint8_t STATUS_REQUEST_READY       = 0x08;
    constexpr uint8_t STATUS_CORRECTED_DATA      = 0x04;
    constexpr uint8_t STATUS_INDEX               = 0x02;
    constexpr uint8_t STATUS_ERROR               = 0x01;

    // Flags in the Error register.
    constexpr uint8_t ERROR_BAD_BLOCK            = 0x80;
    constexpr uint8_t ERROR_UNCORRECTABLE        = 0x40;
    constexpr uint8_t ERROR_MEDIA_CHANGED        = 0x20;
    constexpr uint8_t ERROR_ID_MASK_NOT_FOUND    = 0x10;
    constexpr uint8_t ERROR_MEDIA_CHANGE_REQUEST = 0x08;
    constexpr uint8_t ERROR_ABORTED              = 0x04;
    constexpr uint8_t ERROR_TRACK_0_NOT_FOUND    = 0x02;
    constexpr uint8_t ERROR_NO_ADDRE_MARK        = 0x01;

    // Offsets in the identification space (in uint16_t's).
    constexpr size_t IDENT_DEVICE_TYPE  = 0;
    constexpr size_t IDENT_MODEL        = 27;
    constexpr size_t IDENT_FEATURES     = 49;
    constexpr size_t IDENT_MAX_LBA      = 60;
    constexpr size_t IDENT_COMMAND_SETS = 82;
    constexpr size_t IDENT_MAX_LBA_EXT  = 100;

    constexpr uint32_t COMMAND_SETS_USES_48_BIT = 1 << 26;

    /*
    Base IO port is:
    - BAR0 for the primary channel;
    - BAR2 for the secondary channel.
    Control base IO port is:
    - BAR1 for the primary channel;
    - BAR3 for the secondary channel.
    Bus master IDE base IO port is:
    - BAR4 for the primary channel;
    - BAR4 + 8 for the secondary channel.
    */
    enum class Register {
        // Upper 4 bits are used to distinguish different
        // kinds of registers.

        // Offsets from the base IO port.
        DATA          = 0x00,
        ERROR         = 0x01, // Read only.
        FEATURES      = 0x01, // Write only.
        SECTOR_COUNT  = 0x02,
        LBA0          = 0x03,
        LBA1          = 0x04,
        LBA2          = 0x05,
        DRIVE_SELECT  = 0x06, // Select drive in the channel.
        STATUS        = 0x07, // Read only.
        COMMAND       = 0x07, // Write only.

        // Offsets from the base IO port (I don't know how they work yet).
        SECTOR_COUNT1 = 0x12,
        LBA3          = 0x13,
        LBA4          = 0x14,
        LBA5          = 0x15,

        // Offsets from the the control base IO port.
        ALT_STATUS    = 0x22, // Read only.
        CONTROL       = 0x22, // Write only.
    };

    enum class Direction {
        READ,
        WRITE,
    };

    enum class Command {
        IDENTIFY = 0xec,
        IDENTIFY_PACKET = 0xa1,
    };

    class Channel {
    public:
        constexpr Channel(uint16_t base_port)
            : base_port(base_port), control_base_port(0), bus_master_port(0) {}

        uint16_t read_data() const {
            return inw(base_port);
        }

        uint16_t read_lba12() const {
            return inb(base_port + 5) << 8 |
                   inb(base_port + 4);
        }

        uint8_t read_status() const {
            return inb(base_port + 7);
        }

        uint8_t read_errors() const {
            return inb(base_port + 1);
        }

        void write_sector_count(uint8_t value) const {
            outb(base_port + 2, value);
        }

        void write_lba_28bit(uint32_t value) const {
            outb(base_port + 3, (uint8_t)value);
            outb(base_port + 4, (uint8_t)(value >> 8));
            outb(base_port + 5, (uint8_t)(value >> 16));
        }

        void write_drive_select(uint8_t value) const {
            outb(base_port + 6, value);
        }

        void write_command(Command data) const {
            outb(base_port + 7, (uint8_t)data);
        }

        // Need to add 400ns delays before all the status registers are up to date.
        // https://wiki.osdev.org/ATA_PIO_Mode#400ns_delays
        void delay_400ns() const {
            read_status();
            read_status();
            read_status();
            read_status();
        }

        void wait_request_ready() const {
            while (!(read_status() & (STATUS_REQUEST_READY | STATUS_ERROR))) {
                tiny_delay();
            };
        }

    private:
        uint16_t base_port;
        uint16_t control_base_port;
        uint16_t bus_master_port;
    } channels[2] = { Channel(0x1f0), Channel(0x170) };

    IdentifyResult Device::identify() {
        Channel& channel = channels[(int)channel_type];

        switch (drive_type) {
        case DriveType::MASTER:
            channel.write_drive_select(0xa0);
            break;
        case DriveType::SLAVE:
            channel.write_drive_select(0xa0 | (1 << 4));
            break;
        }
        io_wait();

        channel.write_command(Command::IDENTIFY);
        io_wait();

        channel.delay_400ns();
        if (channel.read_status() == 0) {
            return { IdentifyResultStatus::NoDevice, 0 };
        }

        interface = InterfaceType::ATA;
        bool error = false;
        for (;;) {
            uint8_t status = channel.read_status();
            if (status & STATUS_ERROR) {
                error = true;
                break;
            }
            if (!(status & STATUS_BUSY) && (status & STATUS_REQUEST_READY)) {
                break;
            }
        }

        if (error) {
            uint16_t device_type = channel.read_lba12();
            if (device_type == 0xeb14 || device_type == 0x9669) {
                interface = InterfaceType::ATAPI;
            } else {
                return { IdentifyResultStatus::UnknownDeviceType, 0 };
            }
 
            channel.write_command(Command::IDENTIFY_PACKET);
            io_wait();
        }

        channel.wait_request_ready();
        if ((channel.read_status() & STATUS_ERROR)) {
            uint8_t error_byte = channel.read_errors();
            return { IdentifyResultStatus::RequestError, error_byte };
        };

        uint16_t identification[256];
        for (int i = 0; i < 256; i++) {
            identification[i] = channel.read_data();
        }

        signature = identification[IDENT_DEVICE_TYPE];
        features = identification[IDENT_FEATURES];
        command_sets = *(uint32_t*)(identification + IDENT_COMMAND_SETS);

        if (command_sets & COMMAND_SETS_USES_48_BIT) {
            size = *(uint32_t*)(identification + IDENT_MAX_LBA_EXT);
        } else { // CHS or 28-bit addressing.
            size = *(uint32_t*)(identification + IDENT_MAX_LBA);
        }

        // I have no idea why we have to swap the characters.
        for (int i = 0; i < 20; i ++) {
            model[2 * i] = identification[IDENT_MODEL + i] >> 8;
            model[2 * i + 1] = identification[IDENT_MODEL + i] & 0xff;
        }
        model[40] = '\0';

        return { IdentifyResultStatus::Success, 0 };
    }

    static ide::Device disks[4];
    static size_t disk_count = 0;

    const ide::Device& get_disk(size_t id) {
        return disks[id];
    }

    size_t get_disk_count() {
        return disk_count;
    }

    void init() {
        for (int channel = 0; channel < 2; channel++) {
            for (int drive_type = 0; drive_type < 2; drive_type++) {
                int id = channel * 2 + drive_type;
                ide::Device disk((ide::ChannelType)channel, (ide::DriveType)drive_type);
                IdentifyResult result = disk.identify();

                switch (result.status) {
                case IdentifyResultStatus::Success:
                    disks[disk_count] = disk;
                    disk_count++;
                    break;
                case IdentifyResultStatus::NoDevice:
                    break;
                case IdentifyResultStatus::UnknownDeviceType:
                    printf("Error identifying disk %d: Unknown device type\n", id);
                    break;
                case IdentifyResultStatus::RequestError:
                    printf("Error identifying disk %d: Device returned error code %x\n",
                        id, result.error_byte);
                    break;
                }
            }
        }
    }
}
