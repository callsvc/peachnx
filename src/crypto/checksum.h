#pragma once
#include <array>
#include <mbedtls/sha256.h>

#include <types.h>
namespace Peachnx::Crypto {
    class Checksum {
    public:
        explicit Checksum(const std::array<u8, 32>& target);
        ~Checksum();

        template<typename T>
        bool MatchWith(const T& object) {
            return MatchWith(reinterpret_cast<const u8*>(&object), sizeof(object));
        }
        bool MatchWith(const u8* data, u64 size);
    private:
        std::array<u8, 32> match;
        mbedtls_sha256_context context;
    };
}