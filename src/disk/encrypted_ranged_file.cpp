#include <boost/align/align_up.hpp>
#include <boost/align/align_down.hpp>
#include <boost/container/small_vector.hpp>

#include <disk/encrypted_ranged_file.h>
#include <disk/nca.h>

namespace peachnx::disk {
    constexpr auto ctrSectorSize{0x10};
    constexpr auto xtsSectorSize{0x200};

    EncryptedRangedFile::EncryptedRangedFile(const std::shared_ptr<VirtualFile>& parent, const EncryptContext& ctx, const u64 sector, const u64 offset, const u64 size, const std::filesystem::path& filename) : VirtualFile(filename, DiskAccess::Read, false, offset, size), backing(parent), context(ctx), sectorStart(sector) {
        cipher.emplace(context.type, context.key.data(), 16);

        const auto isCtr{cipher->IsCtrMode()};
        DecryptFuncImpl = isCtr ? &EncryptedRangedFile::DecryptFuncCtr : &EncryptedRangedFile::DecryptFuncXts;
        sectorSize = isCtr ? ctrSectorSize : xtsSectorSize;

        if (isCtr)
            cipher->ResetIv(ctx.nonce);
    }

    u64 EncryptedRangedFile::ReadImpl(const std::span<u8>& output, const u64 offset) {
        if (!output.size())
            return {};
        const auto emplace{offset % sectorSize};
        const auto target{boost::alignment::align_down(readOffset + offset, sectorSize)};

        const auto requested{boost::alignment::align_up(output.size(), sectorSize)};
        const auto blocks{requested / sectorSize};

        if (requested == output.size()) {
            if (backing->Read(output, target) != requested)
                throw std::runtime_error("Failed to read from backing");

            (this->*DecryptFuncImpl)(output, target);
            return requested;
        }
        boost::container::small_vector<u8, ctrSectorSize> block(sectorSize);

        if (requested > block.size())
            block.resize(requested);
        if (backing->Read(block, target) != block.size())
            throw std::runtime_error("Failed to read from backing");

        (this->*DecryptFuncImpl)(block, target);

        u64 result{};
        [[unlikely]] if (emplace) {
            const auto size{blocks * sectorSize + emplace};
            std::memcpy(output.data(), block.data(), size);
            result += size;
            backing->Read(output.subspan(size), offset + size);
        } else {
            std::memcpy(output.data(), block.data(), output.size());
            result += output.size();
        }
        return result;
    }
    void EncryptedRangedFile::UpdateCtrIv(u64 offset) {
        offset >>= 4;
        cipher->SetupIvTweak(offset);
    }
    void EncryptedRangedFile::DecryptFuncXts(const std::span<u8>& content, const u64 offset) {
        const auto counter{offset / sectorSize + sectorStart};
        cipher->DecryptXts(&content[0], content.size(), counter, sectorSize);
    }
    void EncryptedRangedFile::DecryptFuncCtr(const std::span<u8>& content, const u64 offset) {
        UpdateCtrIv(offset);
        cipher->Decrypt(&content[0], &content[0], content.size());
    }
}
