#pragma once
#include <array>
#include <vector>
#include <span>

#include <crypto/keys_types.h>
#include <mbedtls/cipher.h>
namespace Peachnx::Crypto {
    class AesStorage {
    public:
        explicit AesStorage(mbedtls_cipher_type_t type, const u8* key, u64 size);
        ~AesStorage();
        void Decrypt(u8* output, const u8* source, u64 size);
        void ResetIv(const std::array<u8, 16>& value);
        void ResetIv();

        void DecryptXts(PcNotVoid auto& obfuscated, const u64 sector, const u64 stride) {
            DecryptXts(reinterpret_cast<void*>(&obfuscated), sizeof(obfuscated), sector, stride);
        }

        void DecryptXts(void* output, const u64 size, const u64 sector, const u64 stride) {
            DecryptXts(output, output, size, sector, stride);
        }

        void DecryptXts(void* output, void* source, u64 size, u64 sector, u64 stride);
        void SetupIvTweak(u64 update);
        bool IsCtrMode() const;
    private:
        Key128 iVec{};
        static_assert(sizeof(iVec) == MBEDTLS_MAX_IV_LENGTH);

        std::vector<u8> pooled;
        mbedtls_cipher_context_t generic;
    };
}