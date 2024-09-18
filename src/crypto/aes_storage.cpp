#include <optional>
#include <stdexcept>
#include <vector>

#include <boost/endian/conversion.hpp>

#include <crypto/aes_storage.h>
namespace peachnx::crypto {

    AesStorage::AesStorage(const mbedtls_cipher_type_t type, const std::span<u8>& key) {
        mbedtls_cipher_init(&generic);
        if (mbedtls_cipher_setup(&generic, mbedtls_cipher_info_from_type(type)) != 0)
            throw std::runtime_error("Failed to initialize AES context");
        mbedtls_cipher_setkey(&generic, &key[0], key.size() * 8, MBEDTLS_DECRYPT);
    }
    void AesStorage::Decrypt(u8* output, const u8* source, const u64 size) {
        // ReSharper disable once CppTooWideScope
        std::optional<std::vector<u8>> aux;
        u8* destination{};
        // Idea from the late Skyline, thank you very much

        if (output == source) {
            if (size > 1024 * 1024) {
                // ReSharper disable once CppJoinDeclarationAndAssignment
                aux = std::vector<u8>(size);
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

        u64 result;
        if (mbedtls_cipher_get_cipher_mode(&generic) == MBEDTLS_MODE_XTS) {
            mbedtls_cipher_update(&generic, source, size, destination, &result);
        }

        if (result != size)
            throw std::runtime_error("Couldn't decrypt the data");

        if (destination != output)
            std::memcpy(output, destination, result);
    }

    void AesStorage::NextIvTweak(const XtsIv& entropy) {
        const auto done{boost::endian::load_big_u64(&entropy[0])};

        std::memcpy(&iv[8], &done, sizeof(done));
        ResetIv();
    }
    void AesStorage::ResetIv() {
        if (mbedtls_cipher_set_iv(&generic, &iv[0], iv.size()) != 0)
            throw std::runtime_error("Failed to set up the initialization vector");
    }
    void AesStorage::DecryptXts(void* output, void* source, const u64 size, const u64 sector, const u64 stride) {
        if (size && sector)
            if (size % sector)
                throw std::runtime_error("Size must be a multiple of the sector");

        u64 count{};
        for (u64 offset{}; offset < size; offset += sector) {
            NextIvTweak({reinterpret_cast<u8*>(&count), sizeof(count)});
            Decrypt(static_cast<u8*>(output) + offset, static_cast<u8*>(source) + offset, stride);
            offset += stride;
            count++;
        }
    }
}
