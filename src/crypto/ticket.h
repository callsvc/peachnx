#pragma once

#include <disk/virtual_types.h>
#include <crypto/keysdb.h>
namespace peachnx::crypto {
    struct Ticket {
        // https://switchbrew.org/wiki/Ticket
        explicit Ticket(const disk::VirtFilePtr& ticket);
        Ticket() = default;

        std::array<u8, 0x40> issuer;
        std::array<u8, 0x100> titleKeyBlock;
        u8 ticketVersion0; // Always 2 for Switch (ES) Tickets
        u8 ticketKeyType;
        u16 ticketVersion1;
        u8 licenseType;
        u8 masterKeyRevision;
        u16 propertiesBitfield;
        u64 pad0;
        u64 ticketId;
        u64 deviceId;
        Key128 rightsId;
        u32 accountId;
        std::array<u8, 0xc> unknown0;
        std::array<u8, 0x140> unknown1;
    };

    static_assert(sizeof(Ticket) == 0x2C0, "");
}
