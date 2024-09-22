#include <boost/align/align_up.hpp>

#include <disk/nca.h>
#include <disk/encrypted_ranged_file.h>

namespace peachnx::disk {
    constexpr auto ctrSectorSize{0x10};
    constexpr auto xtsSectorSize{0x200};

    EncryptedRangedFile::EncryptedRangedFile(const std::shared_ptr<VirtualFile>& parent, const EncryptContext& ctx, const u64 offset, const u64 size, const std::filesystem::path& filename) : VirtualFile(filename, DiskAccess::Read, false, offset, size), backing(parent), context(ctx) {
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
        const auto sectorOffset{offset % sectorSize};
        auto deltaOffset{readOffset + offset};

        if (!sectorOffset) {
            if (backing->Read(output, deltaOffset) != output.size())
                return {};
            (this->*DecryptFuncImpl)(output, deltaOffset);
            return output.size();
        }

        const auto target{sectorOffset - offset};
        deltaOffset = readOffset + target;
        std::vector<u8> block(sectorSize);
        backing->Read(block, deltaOffset);

        (this->*DecryptFuncImpl)(block, deltaOffset);

        const auto emplace{size + sectorOffset < sectorSize ? size : sectorSize - sectorOffset};
        assert(block.size() - sectorOffset < emplace);
        std::memcpy(output.data(), &block[sectorOffset], emplace);
        u64 result{emplace};

        if (output.size() < result)
            result += backing->Read(output.subspan(result), deltaOffset + emplace);
        return result;
    }
    void EncryptedRangedFile::UpdateCtrIv(u64 offset) {
        offset >>= 4;
        cipher->SetupIvTweak(offset);
    }
    void EncryptedRangedFile::DecryptFuncXts(const std::span<u8>& content, const u64 offset) {
        const auto counter{offset / sectorSize + context.sector};
        cipher->DecryptXts(&content[0], content.size(), counter, sectorSize);
    }
    void EncryptedRangedFile::DecryptFuncCtr(const std::span<u8>& content, const u64 offset) {
        UpdateCtrIv(offset);
        cipher->Decrypt(&content[0], &content[0], content.size());
    }
}
