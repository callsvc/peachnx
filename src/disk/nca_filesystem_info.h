#pragma once

#include <disk/virtual_types.h>

#include <crypto/aes_storage.h>
namespace peachnx::disk {
    constexpr auto FsHeaderVersion{2};
    struct alignas(0x200) NsaFsHeader {
        enum FsType {
            RomFs,
            PartitionFs
        };

        u16 version; // (always 2)
        FsType type;
    };

    class NcaFilesystemInfo {
    public:
        NcaFilesystemInfo(const VirtFilePtr& nca, u32 index);
        VirtFilePtr OpenEncryptedStorage(std::optional<crypto::AesStorage>& storage);
    private:
        NsaFsHeader header;
        u32 section; // Our section index
        u32 offset;
        bool encrypted;
    };
}