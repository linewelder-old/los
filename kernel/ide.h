#pragma once

#include <stdint.h>
#include <stddef.h>

#include "pci.h"

namespace ide {
    enum class ChannelType {
        PRIMARY,
        SECONDARY,
    };

    enum class DriveType {
        MASTER,
        SLAVE,
    };

    enum class InterfaceType {
        ATA,
        ATAPI,
    };

    enum class IdentifyResultStatus {
        Success,
        NoDevice,
        UnknownDeviceType,
        RequestError, // Details are in error_byte.
    };

    struct IdentifyResult {
        IdentifyResultStatus status;
        uint8_t error_byte; // Errors read from the error register.
    };

    class Device {
    public:
        constexpr Device()
            : channel_type(ChannelType::PRIMARY), drive_type(DriveType::MASTER) {}
        constexpr Device(ChannelType channel_type, DriveType drive_type)
            : channel_type(channel_type), drive_type(drive_type) {}

        IdentifyResult identify();

        ChannelType channel_type;
        DriveType drive_type;
        InterfaceType interface = InterfaceType::ATA;
        uint16_t signature = 0; // Drive signature.
        uint16_t features = 0;
        uint32_t command_sets = 0; // Command sets supported.
        uint32_t size = 0; // Size in sectors.
        char model[41] = "";
    };

    void init(const pci::Function& func);

    const ide::Device& get_disk(size_t id);
    size_t get_disk_count();
}
