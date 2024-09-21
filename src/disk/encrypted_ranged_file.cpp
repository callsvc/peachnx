#include <boost/align/align_up.hpp>

#include <disk/nca.h>
#include <disk/encrypted_ranged_file.h>

namespace peachnx::disk {
    constexpr auto ctrSectorSize{0x10};
    constexpr auto xtsSectorSize{0x200};

    EncryptedRangedFile::EncryptedRangedFile(const std::shared_ptr<VirtualFile>& parent, const EncryptContext& ctx, const u64 offset, const u64 size, const std::filesystem::path& filename) : VirtualFile(filename, DiskAccess::Read, false, offset, size), backing(parent), context(ctx) {
        cipher.emplace(context.type, context.key);

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
        if (!sectorOffset) {
            assert(output.size() % sectorSize);

            if (backing->Read(output, readOffset + offset) != output.size())
                return {};
            (this->*DecryptFuncImpl)(output, offset);
            return output.size();
        }
        const auto target{sectorOffset - offset};
        backing->Read(output, readOffset + target);

        std::vector<u8> placeable(sectorSize);
        (this->*DecryptFuncImpl)(placeable, target);
        if (size + sectorOffset < sectorSize) {
            std::memcpy(output.data(), &placeable[sectorOffset], size);
        }
        return output.size();
    }
    void EncryptedRangedFile::UpdateCtrIv(u64 offset) {
        offset += backing->GetOffset();
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
