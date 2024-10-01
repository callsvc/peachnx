#pragma once

#include <disk/virtual_types.h>
#include <crypto/keysdb.h>
#include <crypto/aes_storage.h>

#include <loader/loader.h>
#include <disk/partition_filesystem.h>
#include <loader/nca_filesystem_info.h>

namespace peachnx::loader {
    enum DistributionType : u8 {
        Download,
        Gamecard
    };
    enum class ContentType : u8 {
        Program,
        Meta,
        Control,
        Manual,
        Data,
        PublicData
    };

    constexpr auto fsEntriesMaxCount{4};
#pragma pack(push, 1)

    struct NcaHeader {
        std::array<u8, 0x100> rsa;
        std::array<u8, 0x100> rsa2; // using a key from NPDM (or zeroes if not a program)
        u32 magic;
        DistributionType distributionType;
        ContentType contentType;
        u8 keyGeneration0;
        crypto::IndexedKey128Type keyIndexType;
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
        std::array<std::array<u8, 0x10>, 4> encryptedKeyArea;
        std::array<u8, 0xc0> pad1;
    };
    static_assert(sizeof(NcaHeader) == 0x400);

#pragma pack(pop)
    class NCA final : public Loader {
    public:
        explicit NCA(const std::shared_ptr<crypto::KeysDb>& kdb, const disk::VirtFilePtr& content);

        static ApplicationType GetTypeFromFile(const disk::VirtFilePtr& probFile);

        crypto::Key128 ReadExternalKey(EncryptionType type) const;
        bool VerifyNcaIntegrity();

        auto& GetDirectories() {
            return dirs;
        }

        std::optional<crypto::AesStorage> cipher;
        ContentType type;
    private:
        void ReadPartitionFs(const disk::VirtFilePtr& content);
        void ReadRomFs(const disk::VirtFilePtr& content);

        void ReadContent(const disk::VirtFilePtr& content);
        u64 GetGenerationKey() const;
        u32 GetFsEntriesCount() const;

        std::shared_ptr<crypto::KeysDb> keys;
        disk::VirtFilePtr nca;

        std::vector<disk::VirtFilePtr> files;
        disk::VirtFilePtr romFs;

        std::vector<std::shared_ptr<disk::PartitionFilesystem>> dirs;
        std::shared_ptr<disk::PartitionFilesystem> exeFs;
        std::shared_ptr<disk::PartitionFilesystem> cnmt;
        std::shared_ptr<disk::PartitionFilesystem> logo;

        NcaHeader header;
        u32 version{};
        bool verified{};
    };
}