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
        if (requested == output.size()) {
            if (backing->Read(output, target) != requested)
                throw std::runtime_error("Failed to read from backing");
            (this->*DecryptFuncImpl)(output, target);
            return requested;
        }
        boost::container::small_vector<u8, ctrSectorSize> buffer(requested);
        auto result{backing->Read(buffer, target)};

        if (result < buffer.size()) {
            if (!result)
                throw std::runtime_error("Failed to read from backing");
            buffer.resize(boost::alignment::align_down(result, sectorSize));
        }
        result = output.size();
        (this->*DecryptFuncImpl)(buffer, target);
        [[unlikely]] if (emplace) {
            assert(buffer.size() == output.size() + emplace);
            std::memcpy(output.data(), &buffer[emplace], output.size());
        } else {
            std::memcpy(output.data(), buffer.data(), output.size());
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
