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
    enum FsType : u8 {
        RomFs,
        PartitionFs
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
    enum class EncryptionType : u8 {
        Auto,
        None,
        AesXts,
        AesCtr,
        AesCtrEx,
        AesCtrSkipLayerHash,
        AesCtrExSkipLayerHash
    };
    constexpr auto fsEntriesMaxCount{4};
#pragma pack(push, 1)
    struct FsEntry {
        u32 startSector;
        u32 endSector;
        u32 hashSectors;
        u32 pad0;
    };

    struct NcaHeader {
        std::array<u8, 0x100> rsa;
        std::array<u8, 0x100> rsa2; // using a key from NPDM (or zeroes if not a program)
        u32 magic;
        DistributionType distributionType;
        ContentType contentType;
        u8 keyGeneration0;
        KeyAreaEncryptionKeyIndex keyArea;
        u64 size; // Size of this NCA
        u64 programId;
        u32 contentIndex;
        u32 sdkAddonVersion;
        u8 keyGeneration1;
        u8 signatureKeyGeneration;
        std::array<u8, 0xe> pad0;
        crypto::Key128 rightsId;

        std::array<FsEntry, fsEntriesMaxCount> entries;
        std::array<std::array<u8, 32>, fsEntriesMaxCount> fsHeaderHashes; // Array of SHA256 hashes (over each FsHeader)
        std::array<u8, 0x40> encryptedKeyArea;
        std::array<u8, 0xc0> pad1;
    };
    static_assert(sizeof(NcaHeader) == 0x400);

#pragma pack(pop)
    class NCA {
    public:
        explicit NCA(const crypto::KeysDb& keysDb, const VirtFilePtr& nca);

        NcaHeader header;
        u32 version{};
    private:
        void ReadContent(const VirtFilePtr& nca) const;

        u32 GetFsEntriesCount() const;
    };
}