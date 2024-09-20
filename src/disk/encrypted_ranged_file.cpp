#include <boost/align/align_up.hpp>

#include <disk/nca.h>
#include <disk/encrypted_ranged_file.h>

namespace peachnx::disk {
    constexpr auto ctrSectorSize{0x10};
    constexpr auto xtsSectorSize{0x200};

    EncryptedRangedFile::EncryptedRangedFile(const std::shared_ptr<VirtualFile>& parent, const EncryptContext& ctx,
        const u64 offset, const u64 size, const std::filesystem::path& filename) :
        VirtualFile(filename, DiskAccess::Read, false, offset, size), backing(parent), context(ctx) {

        std::span<u8> key;
        if (context.type == MBEDTLS_CIPHER_AES_128_CTR) {
            key = std::span(context.closureKey).subspan(0, ctrSectorSize);
            sectorSize = ctrSectorSize;
        } else {
            key = context.closureKey;
            sectorSize = xtsSectorSize;
        }
        decrypt = crypto::AesStorage(context.type, key);
    }
    u64 EncryptedRangedFile::ReadImpl(const std::span<u8>& output, const u64 offset) {
        const auto targetCount{boost::alignment::align_up(output.size(), sectorSize)};
        auto content{backing->GetBytes(targetCount, rd + offset)};

        const auto counter{offset / sectorSize + context.sector};
        switch (context.type) {
            case MBEDTLS_CIPHER_AES_128_XTS:
                decrypt->DecryptXts(content.data(), content.size(), counter, sectorSize);
            default: {}
        }
        if (content.size() < output.size()) {
            throw std::runtime_error("Unable to read one or more sectors of the file");
        }
        std::memcpy(&output[0], &content[0], output.size());

        return output.size();
    }

}
