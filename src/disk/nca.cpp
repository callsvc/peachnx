#include <cassert>
#include <ranges>

#include <disk/nca.h>
#include <disk/nca_filesystem_info.h>
#include <crypto/aes_storage.h>
namespace peachnx::disk {
    constexpr auto sdkMinimumVersion{0x000b0000};
    NCA::NCA(const crypto::KeysDb& keysDb, const VirtFilePtr& nca) {
        if (!nca)
            return;
        header = nca->Read<NcaHeader>();
        if (header.magic != MakeMagic<u32>("NCA3")) {
            if (!keysDb.headerKey)
                throw std::runtime_error("Content key inaccessible");
            auto titleKey{*keysDb.headerKey};
            crypto::AesStorage storage(MBEDTLS_CIPHER_AES_128_XTS, titleKey);
            storage.DecryptXts<NcaHeader>(header, 0, 0x200);

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

        // ReSharper disable once CppIfCanBeReplacedByConstexprIf
        if (!header.rightsId.empty()) {
            std::array<u64, 2> rightsPair;
            std::memcpy(&rightsPair[0], &header.rightsId[0], sizeof(rightsPair));
        }

        ReadContent(nca);
    }
    void NCA::ReadContent(const VirtFilePtr& nca) const {
        for (u32 entry{}; entry < GetFsEntriesCount(); entry++) {
            [[maybe_unused]] NcaFilesystemInfo fsInfo{nca, entry};
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
