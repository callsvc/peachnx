#pragma once

#include <disk/virtual_types.h>
#include <crypto/keysdb.h>
namespace peachnx::disk {
    struct alignas(1024) NcaHeader {
        std::array<u8, 0x100> rsa;
        std::array<u8, 0x100> rsa2; // using a key from NPDM (or zeroes if not a program)
        u32 magic;
    };
    class NCA {
    public:
        explicit NCA(const crypto::KeysDb& keysDb, const VirtFilePtr& nca);

        NcaHeader header;
        u32 version{};
    };
}