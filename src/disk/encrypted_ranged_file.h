#pragma once

#include <disk/virtual_file.h>
#include <crypto/aes_storage.h>
namespace peachnx::disk {
    struct EncryptContext {
        mbedtls_cipher_type_t type;
        crypto::Key256 closureKey; // The CTR key is also encoded here

        u64 sector;
    };
    class EncryptedRangedFile final : public VirtualFile {
    public:
        EncryptedRangedFile(const std::shared_ptr<VirtualFile>& parent, const EncryptContext& ctx, u64 offset, u64 size, const std::filesystem::path& filename);

        u64 ReadImpl(const std::span<u8>& output, u64 offset) override;

        const std::shared_ptr<VirtualFile>& backing;
        EncryptContext context;
        std::optional<crypto::AesStorage> decrypt;
        u64 sectorSize{};
    };
}