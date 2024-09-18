#pragma once

#include <disk/virtual_types.h>
#include <crypto/keysdb.h>
namespace peachnx::disk {
    enum DistributionType : u8 {
        Download,
        Gamecard
    };
    enum ContentType : u8 {
        Program,
        Meta,
        Control,
        Manual,
        Data,
        PublicData
    };
    enum KeyAreaEncryptionKeyIndex : u8 {
        Application,
        Ocean,
        System
    };
#pragma pack(push, 1)
    struct alignas(1024) NcaHeader {
        std::array<u8, 0x100> rsa;
        std::array<u8, 0x100> rsa2; // using a key from NPDM (or zeroes if not a program)
        u32 magic;
        DistributionType distributionType;
        ContentType contentType;
        u8 keyGeneration0;
        KeyAreaEncryptionKeyIndex keyArea;
        u64 contentSize;
        u64 programId;
        u32 contentIndex;
        u32 sdkAddonVersion;
        u8 keyGeneration1;
    };
#pragma pack(pop)
    class NCA {
    public:
        explicit NCA(const crypto::KeysDb& keysDb, const VirtFilePtr& nca);

        NcaHeader header;
        u32 version{};
    };
}