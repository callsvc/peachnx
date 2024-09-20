#include <cassert>
#include <ranges>

#include <disk/nca_filesystem_info.h>
#include <disk/nca.h>
namespace peachnx::disk {
    constexpr auto sdkMinimumVersion{0x000b0000};
    auto CheckContentArchiveMagic(const u32 magic) {
        static std::vector<u32> versions;
        for (u32 content{}; content < 4; content++) {
            if (versions.size() == 4)
                break;
            std::stringstream fmtContent;
            fmtContent << std::format("NCA{}", content);
            versions.push_back(MakeMagic<u32>(fmtContent.str()));
        }
        const auto ncaId{std::ranges::find(versions, magic)};
        return ncaId != versions.end() ? *ncaId : 0;
    }


    NCA::NCA(const crypto::KeysDb& keysDb, const VirtFilePtr& nca) {
        if (!nca)
            return;
        header = nca->Read<NcaHeader>();

        u32 archive;
        if ((archive = CheckContentArchiveMagic(header.magic)) == 0) {
            auto titleKey{*keysDb.headerKey};
            storage = crypto::AesStorage(MBEDTLS_CIPHER_AES_128_XTS, titleKey);
        }
        if (archive != MakeMagic<u32>("NCA3")) {
            if (!keysDb.headerKey)
                throw std::runtime_error("Content key inaccessible");

            storage->DecryptXts<NcaHeader>(header, 0, 0x200);
            archive == CheckContentArchiveMagic(header.magic);

            if (header.sdkAddonVersion < sdkMinimumVersion)
                throw std::runtime_error("SDK addon version too small");
        }
        // Checking if the user-provided keys meet the requirements to proceed
        const auto keyGeneration{std::max(header.keyGeneration0, header.keyGeneration1) - 1};
        const auto keyIndex{header.keyArea};
        if (!keysDb.Exists(keyGeneration, keyIndex)) {
            throw std::runtime_error("Key area not found");
        }

        assert(nca->GetSize() == header.size);

        if (header.rightsId != crypto::Key128{}) {
            std::array<u64, 2> rightsPair;
            std::memcpy(&rightsPair[0], &header.rightsId[0], sizeof(rightsPair));
        }

        ReadContent(nca);
    }
    void NCA::ReadContent(const VirtFilePtr& nca) {
        for (u32 entry{}; entry < GetFsEntriesCount(); entry++) {
            NcaFilesystemInfo fsInfo{nca, entry};
            [[maybe_unused]] auto backing{fsInfo.OpenEncryptedStorage(storage)};
        }
    }
    u32 NCA::GetFsEntriesCount() const {
        for (const auto& [index, antGroup] : std::views::enumerate(header.entries)) {
            if (!antGroup.startSector && !antGroup.endSector)
                return index;
        }
        return {};
    }
}
