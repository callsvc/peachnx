#pragma once

#include <disk/virtual_file.h>
#include <crypto/aes_storage.h>
namespace peachnx::disk {
    struct EncryptContext {
        mbedtls_cipher_type_t type;
        crypto::Key256 key;

        u64 sector;
        crypto::Key128 nonce;
    };
    class EncryptedRangedFile final : public VirtualFile {
    public:
        EncryptedRangedFile(const std::shared_ptr<VirtualFile>& parent, const EncryptContext& ctx, u64 offset, u64 size, const std::filesystem::path& filename);

    private:
        u64 ReadImpl(const std::span<u8>& output, u64 offset) override;
        void UpdateCtrIv(u64 offset);

        const std::shared_ptr<VirtualFile>& backing;
        std::optional<crypto::AesStorage> cipher;
        EncryptContext context;
        u64 sectorSize{};
    };
}