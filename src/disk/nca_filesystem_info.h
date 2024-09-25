#pragma once

#include <disk/virtual_types.h>
namespace peachnx::disk {
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

#pragma pack(push, 1)

    struct FsEntry {
        u32 startSector;
        u32 endSector;   // (in blocks which are 0x200 bytes)
        u32 contentHash;
        u32 pad0;
    };
    struct VerificationLevelInformation {
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
        std::array<VerificationLevelInformation, 6> levels;
        std::array<u8, 0x20> pad1;
        std::array<u8, 0x20> masterHash;
        std::array<u8, 0x18> pad2;
    };

    struct HierarchicalSha256Data {
        std::array<u8, 0x20> hashTableHash; // Hash over the hash table at the beginning of the data section
        u32 blockSize;
        u32 layerCount; // (always 2)
        struct Region {
            u64 offset;
            u64 size;
        };
        std::array<Region, 5> layers;
        std::array<u8, 0x80> pad1;
    };

    struct alignas(0x200) FsHeader {
        enum FsType : u8 {
            RomFs,
            PartitionFs
        };

        u16 version; // (always 2)
        FsType type;
        HashType hashType;
        EncryptionType encryptionType;
        bool hasIntegrity;
        u16 pad0;
        union {
            IntegrityMetaInfo integrity;
            HierarchicalSha256Data sha256Data;

            static_assert(sizeof(integrity) == 0xf8);
        };
        std::array<u8, 0x40> patchInfo;
        union {
            u64 nonce;

            struct {
                u32 generation;
                u32 secureValue;
            };
        };
    };
    static_assert(sizeof(FsHeader) == 0x200);
#pragma pack(pop)

    class NCA;

    struct ContentFsInfo {
        ContentFsInfo() = default;
        explicit ContentFsInfo(const FsEntry& fsInfo);
        u64 GetSector(bool isOffset = true) const;
        u64 offset;
        u64 size;
    };

    class NcaFilesystemInfo {
    public:
        NcaFilesystemInfo(const VirtFilePtr& nca, const FsEntry& fsInfo, u32 index);
        VirtFilePtr MountEncryptedFile(const std::array<u8, 32>& value, NCA& nca);
        std::string GetFileName() const;
        void FixupOffsetAndSize();

        bool isPartition{};
    private:
        FsHeader header;
        const VirtFilePtr& parent;
        u32 section; // Our section index
        ContentFsInfo cfs;

        FsEntry entry;
        bool encrypted;
    };
}