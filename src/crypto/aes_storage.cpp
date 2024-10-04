#include <cassert>
#include <optional>
#include <stdexcept>
#include <vector>

#include <boost/endian/conversion.hpp>

#include <crypto/aes_storage.h>
namespace Peachnx::Crypto {

    AesStorage::AesStorage(const mbedtls_cipher_type_t type, const u8* key, const u64 size) {
        mbedtls_cipher_init(&generic);
        if (mbedtls_cipher_setup(&generic, mbedtls_cipher_info_from_type(type)) != 0)
            throw std::runtime_error("Failed to initialize AES context");

        if (const auto keyResult = mbedtls_cipher_setkey(&generic, key, size * 8, MBEDTLS_DECRYPT))
            if (keyResult == MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA)
                throw std::runtime_error("Key size in bits incorrectly specified for the current type");
    }
    AesStorage::~AesStorage() {
        assert(mbedtls_cipher_get_key_bitlen(&generic));
        mbedtls_cipher_free(&generic);
    }

    void AesStorage::Decrypt(u8* output, const u8* source, const u64 size) {
        // ReSharper disable once CppTooWideScope
        std::optional<std::vector<u8>> aux;
        u8* destination{};
        // Idea from the late Skyline, thank you very much

        if (output == source) {
            if (size > 1024 * 1024) {
                aux.emplace(size);
                destination = &(*aux)[0];
            } else {
                if (pooled.size() < size) {
                    pooled.resize(size);
                }
                destination = &pooled[0];
            }
        } else {
            destination = output;
        }
        if (!destination)
            throw std::runtime_error("We don't have a buffer selected");
        mbedtls_cipher_reset(&generic);

        u64 result{};
        if (mbedtls_cipher_get_cipher_mode(&generic) == MBEDTLS_MODE_XTS) {
            mbedtls_cipher_update(&generic, source, size, destination, &result);
        } else {
            const auto blockSize{mbedtls_cipher_get_block_size(&generic)};
            if (size < blockSize) {
                std::vector<u8> block(blockSize);
                if (block.empty())
                    throw std::runtime_error("Block is empty");
                std::memcpy(&block[0], source, size);
                Decrypt(&block[0], &block[0], block.size());
                std::memcpy(destination, &block[0], size);

                return;
            }

            // https://github.com/Mbed-TLS/mbedtls/blob/2ca6c285a0dd3f33982dd57299012dacab1ff206/library/aes.c#L1453
            for (u64 offset{}; offset < size; offset += blockSize) {
                u64 processed{};

                const auto length{std::min<u64>(blockSize, size - offset)};
                mbedtls_cipher_update(&generic, &source[offset], length, &destination[offset], &processed);
                result += processed;
            }
        }

        if (result != size)
            throw std::runtime_error("Couldn't decrypt the data");

        if (destination != output)
            std::memcpy(output, destination, result);
    }
    void AesStorage::ResetIv(const std::array<u8, 16>& value) {
        std::memcpy(&iVec[0], &value[0], sizeof(iVec));
    }

    void AesStorage::ResetIv() {
        if (mbedtls_cipher_set_iv(&generic, &iVec[0], iVec.size()) != 0)
            throw std::runtime_error("Failed to set up the initialization vector");
    }
    void AesStorage::DecryptXts(void* output, void* source, const u64 size, u64 sector, const u64 stride) {
        if (size && stride)
            if (size % stride)
                throw std::runtime_error("Size must be a multiple of the sector");

        for (u64 offset{}; offset < size; offset += stride) {
            SetupIvTweak(sector);
            Decrypt(static_cast<u8*>(output) + offset, static_cast<u8*>(source) + offset, stride);
            sector++;
        }
    }

    // https://gist.github.com/SciresM/fe8a631d13c069bd66e9c656ab5b3f7f
    void AesStorage::SetupIvTweak(const u64 update) {
        const auto nintendo{boost::endian::native_to_big(update)};

        std::memcpy(&iVec[8], &nintendo, sizeof(nintendo));
        ResetIv();
    }
    bool AesStorage::IsCtrMode() const {
        return mbedtls_cipher_get_cipher_mode(&generic) == MBEDTLS_MODE_CTR;
    }
}
