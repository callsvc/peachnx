#pragma once

#include <sys_fs/virtual_file.h>
#include <crypto/aes_storage.h>
namespace Peachnx::SysFs {
    struct EncryptContext {
        mbedtls_cipher_type_t type;
        Crypto::Key128 key;
        Crypto::Key128 nonce;
    };
    class EncryptedRangedFile final : public VirtualFile {
    public:
        EncryptedRangedFile(const std::shared_ptr<VirtualFile>& parent, const EncryptContext& ctx, u64 sector, u64 offset, u64 size, const std::filesystem::path& filename);

    private:
        u64 ReadImpl(const std::span<u8>& output, u64 offset) override;
        void UpdateCtrIv(u64 offset);
        void DecryptFuncXts(const std::span<u8>& content, u64 offset);
        void DecryptFuncCtr(const std::span<u8>& content, u64 offset);

        const std::shared_ptr<VirtualFile>& backing;
        std::optional<Crypto::AesStorage> cipher;
        EncryptContext context;

        u64 sectorSize{};
        u64 sectorStart{};
        using DecryptFuncCallback = void (EncryptedRangedFile::*)(const std::span<u8>&, u64 offset);
        DecryptFuncCallback DecryptFuncImpl{};
    };
}