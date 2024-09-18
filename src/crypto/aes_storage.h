#pragma once
#include <array>
#include <vector>
#include <span>

#include <crypto/keys_types.h>
#include <mbedtls/cipher.h>
namespace peachnx::crypto {
    using XtsIv = std::span<u8>;
    class AesStorage {
    public:
        explicit AesStorage(mbedtls_cipher_type_t type, const std::span<u8>& key);
        void Decrypt(u8* output, const u8* source, u64 size);
        void NextIvTweak(const XtsIv& entropy);
        void ResetIv();

        void DecryptXts(PcNotVoid auto& obfuscated, const u64 sector, const u64 stride) {
            DecryptXts(reinterpret_cast<void*>(&obfuscated), sizeof(obfuscated), sector, stride);
        }

        void DecryptXts(void* output, const u64 size, const u64 sector, const u64 stride) {
            DecryptXts(output, output, size, sector, stride);
        }

        void DecryptXts(void* output, void* source, u64 size, u64 sector, u64 stride);

    private:
        Key128 iv{};

        std::vector<u8> pooled;
        mbedtls_cipher_context_t generic;
    };
}