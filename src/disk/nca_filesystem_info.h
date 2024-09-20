#pragma once

#include <disk/virtual_types.h>
#include <crypto/keysdb.h>
#include <crypto/aes_storage.h>
namespace peachnx::disk {
#pragma pack(push, 1)
    struct FsEntry {
        u32 startSector;
        u32 endSector;   // (in blocks which are 0x200 bytes)
        u32 contentHash;
        u32 pad0;
    };
#pragma pack(pop)

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

    constexpr auto FsHeaderVersion{2};
    struct alignas(0x200) NsaFsHeader {
        enum FsType : u8 {
            RomFs,
            PartitionFs
        };

        u16 version; // (always 2)
        FsType type;
        HashType hashType;
        EncryptionType encryptionType;
    };

    class NcaFilesystemInfo {
    public:
        NcaFilesystemInfo(const VirtFilePtr& nca, const FsEntry& detail, u32 index);
        VirtFilePtr OpenEncryptedStorage(const crypto::KeysDb& keysDb, std::optional<crypto::AesStorage>& storage);
        std::string GetEntryName() const;

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