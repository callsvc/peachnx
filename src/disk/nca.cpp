#include <cassert>
#include <ranges>

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


    NCA::NCA(const crypto::KeysDb& keysDb, const VirtFilePtr& nca) : keys(keysDb) {
        if (!nca)
            return;
        header = nca->Read<NcaHeader>();

        u32 archive;
        if ((archive = CheckContentArchiveMagic(header.magic)) == 0) {
            cipher.emplace(MBEDTLS_CIPHER_AES_128_XTS, keys.headerKey->data(), keys.headerKey->size());
        }
        if (archive != MakeMagic<u32>("NCA3")) {
            if (!keys.headerKey)
                throw std::runtime_error("Content key inaccessible");

            cipher->DecryptXts<NcaHeader>(header, 0, 0x200);
            archive == CheckContentArchiveMagic(header.magic);

            if (header.sdkAddonVersion < sdkMinimumVersion)
                throw std::runtime_error("SDK addon version too small");
        }
        // Checking if the user-provided keys meet the requirements to proceed
        const auto keyIndex{header.keyAreaType};
        if (!keys.GetKey(GetGenerationKey(), keyIndex)) {
            throw std::runtime_error("Key area not found");
        }

        assert(nca->GetSize() == header.size);

        ReadContent(nca);
    }
    void NCA::ReadContent(const VirtFilePtr& nca) {
        for (u32 entry{}; entry < GetFsEntriesCount(); entry++) {
            NcaFilesystemInfo fsInfo{nca, header.entries[entry], entry};
            const auto backing{fsInfo.MountEncryptedFile(*this)};

            [[maybe_unused]] const auto content{backing->GetBytes(512)};
            const u32 magic{backing->Read<u32>()};
            if (fsInfo.isPartition)
                assert(magic == MakeMagic<u32>("PFS0"));
        }
    }
    u64 NCA::GetGenerationKey() const {
        auto result{std::max<u64>(header.keyGeneration0, header.keyGeneration1)};
        result = std::max<u64>(result, 1) - 1;
        return result;
    }
    u32 NCA::GetFsEntriesCount() const {
        for (const auto& [index, antGroup]: std::views::enumerate(header.entries)) {
            if (!antGroup.startSector && !antGroup.endSector)
                return index;
        }
        return {};
    }
    crypto::Key128 NCA::ReadExternalKey(const EncryptionType type) const {
        crypto::Key128 result;
        std::optional<crypto::Key128> ecbKey;

        if (crypto::KeyIsEmpty(header.rightsId)) {
            constexpr std::array taggedKeys{"application", "ocean", "system"};

            const auto keyIndex = [&] -> u64 {
                switch (type) {
                    case EncryptionType::AesCtr:
                        return 2;
                    case EncryptionType::AesXts:
                    default:
                        return {};
                }
            }();
            ecbKey = keys.GetKey(taggedKeys[header.keyAreaType], GetGenerationKey());
            result = crypto::Key128{header.encryptedKeyArea[keyIndex]};
        } else {
            if (const auto title{keys.GetTitle(header.rightsId)})
                result = *title;

            ecbKey = keys.GetKey("titlekek_", GetGenerationKey());
        }
        if (!ecbKey)
            throw std::runtime_error("Title/Area key missing");

        crypto::AesStorage cipher(MBEDTLS_CIPHER_AES_128_ECB, ecbKey->data(), ecbKey->size());
        cipher.Decrypt(result.data(), result.data(), sizeof(result));
        return result;
    }
}
