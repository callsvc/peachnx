#include <boost/align/align_up.hpp>

#include <disk/nca.h>
#include <disk/encrypted_ranged_file.h>

namespace peachnx::disk {
    constexpr auto ctrSectorSize{0x10};
    constexpr auto xtsSectorSize{0x200};

    EncryptedRangedFile::EncryptedRangedFile(const std::shared_ptr<VirtualFile>& parent, const EncryptContext& ctx, const u64 offset, const u64 size, const std::filesystem::path& filename) : VirtualFile(filename, DiskAccess::Read, false, offset, size), backing(parent), context(ctx) {
        cipher.emplace(context.type, context.key);

        if (context.type == MBEDTLS_CIPHER_AES_128_CTR) {
            sectorSize = ctrSectorSize;
            cipher->ResetIv(ctx.nonce);
        } else {
            sectorSize = xtsSectorSize;
        }
    }
    u64 EncryptedRangedFile::ReadImpl(const std::span<u8>& output, const u64 offset) {
        const auto target{boost::alignment::align_up(output.size(), sectorSize)};
        auto content{backing->GetBytes(target, readOffset + offset)};

        if (content.size() < output.size()) {
            throw std::runtime_error("Unable to read one or more sectors of the file");
        }
        if (context.type == MBEDTLS_CIPHER_AES_128_XTS) {
            const auto counter{offset / sectorSize + context.sector};
            cipher->DecryptXts(&content[0], content.size(), counter, sectorSize);
        }

        if (context.type == MBEDTLS_CIPHER_AES_128_CTR) {
            UpdateCtrIv(offset);
            cipher->Decrypt(&content[0], &content[0], content.size());
        }
        std::memcpy(&output[0], &content[0], output.size());
        return output.size();
    }
    void EncryptedRangedFile::UpdateCtrIv(u64 offset) {
        offset += backing->GetOffset();
        offset >>= 4;
        cipher->SetupIvTweak(offset);
    }

}
