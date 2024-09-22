#pragma once

#include <disk/virtual_types.h>
namespace peachnx::disk {
#pragma pack(push, 1)

    struct FsEntry {
        u32 startSector;
        u32 endSector;   // (in blocks which are 0x200 bytes)
        u32 contentHash;
        u32 pad0;
    };

    enum class EncryptionType : u8 {
        Auto,
        None,
        AesXts,
        AesCtr,
        AesCtrEx,
        AesCtrSkipLayerHash,
        AesCtrExSkipLayerHash
    };
    enum class HashType : u8 {
        Auto,
        None,
        HierarchicalSha256Hash,
        HierarchicalIntegrityHash,
        AutoSha3,
        HierarchicalSha3256Hash,
        HierarchicalIntegritySha3Hash
    };

    constexpr auto fsHeaderVersion{2};

    struct IntegrityVerification {
        u64 offset;
        u64 size;
        u32 blockSize; // (in log2)
        u32 pad0;
    };

    struct IntegrityMetaInfo {
        u32 magic; // ("IVFC")
        u32 version;
        u32 masterHashSize;

        u32 levelsCount;
        std::array<IntegrityVerification, 6> levels;
        std::array<u8, 0x20> pad0;
        std::array<u8, 0x20> masterHash;
        std::array<u8, 0x18> pad1;
    };

    struct alignas(0x200) NsaFsHeader {
        enum FsType : u8 {
            RomFs,
            PartitionFs
        };

        u16 version; // (always 2)
        FsType type;
        HashType hashType;
        EncryptionType encryptionType;
        u16 pad0;
        union {
            IntegrityMetaInfo integrity;

            static_assert(sizeof(integrity) == 0xf8);
        };
        std::array<u8, 0x40> patchInfo;
        union {
            u64 nonce;

            u32 generation;
            u32 secureValue;
        };
    };
#pragma pack(pop)

    class NCA;

    class NcaFilesystemInfo {
    public:
        NcaFilesystemInfo(const VirtFilePtr& nca, const FsEntry& fsInfo, u32 index);
        VirtFilePtr MountEncryptedFile(NCA& nca);
        std::string GetFileName() const;

        bool isPartition{};
    private:
        NsaFsHeader header;
        const VirtFilePtr& parent;
        u32 section; // Our section index
        u32 offset;

        FsEntry entry;
        u32 count;
        bool encrypted;
    };
}