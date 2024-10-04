#pragma once

#include <sys_fs/virtual_types.h>
#include <crypto/keysdb.h>
#include <crypto/aes_storage.h>

#include <sys_fs/partition_filesystem.h>
#include <loader/loader.h>
#include <loader/nca_mount.h>

namespace Peachnx::Loader {
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
        Crypto::IndexedKey128Type keyIndexType;
        u64 size; // Size of this NCA
        u64 programId;
        u32 contentIndex;
        u32 sdkAddonVersion;
        u8 keyGeneration1;
        u8 signatureKeyGeneration;
        std::array<u8, 0xe> pad0;
        Crypto::Key128 rightsId;

        std::array<FsEntry, fsEntriesMaxCount> entries;
        std::array<std::array<u8, 32>, fsEntriesMaxCount> fsHeaderHashes; // Array of SHA256 hashes (over each FsHeader)
        std::array<std::array<u8, 0x10>, 4> encryptedKeyArea;
        std::array<u8, 0xc0> pad1;
    };
    static_assert(sizeof(NcaHeader) == 0x400);

#pragma pack(pop)
    class NCA final : public ComposedLoader {
    public:
        explicit NCA(const std::shared_ptr<Crypto::KeysDb>& kdb, const SysFs::VirtFilePtr& content);

        Crypto::Key128 ReadExternalKey(EncryptionType type) const;
        bool VerifyNcaIntegrity();

        auto& GetDirectories() {
            return dirs;
        }

        u64 GetProgramId() const;
        bool CheckIntegrity() const override;

        std::optional<Crypto::AesStorage> cipher;
        std::shared_ptr<SysFs::PartitionFilesystem> exeFs;
        std::shared_ptr<SysFs::PartitionFilesystem> cnmt;
        std::shared_ptr<SysFs::PartitionFilesystem> logo;

        ContentType type;
    private:
        void ReadPartitionFs(const SysFs::VirtFilePtr& content);
        void ReadRomFs(const SysFs::VirtFilePtr& content);

        void ReadContent(const SysFs::VirtFilePtr& content);
        u64 GetGenerationKey() const;
        u32 GetFsEntriesCount() const;

        std::shared_ptr<Crypto::KeysDb> keys;
        SysFs::VirtFilePtr nca;

        std::vector<SysFs::VirtFilePtr> files;
        SysFs::VirtFilePtr romFs;

        std::vector<std::shared_ptr<SysFs::PartitionFilesystem>> dirs;

        NcaHeader header;
        u32 version{};
        bool verified{};
    };
}