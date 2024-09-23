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
        const auto keyIndex{header.keyIndexType};
        if (!keys.GetKey(crypto::Application, GetGenerationKey(), keyIndex)) {
            throw std::runtime_error("Key area not found");
        }

        assert(nca->GetSize() == header.size);

        ReadContent(nca);
    }
    void NCA::ReadContent(const VirtFilePtr& nca) {
        for (u32 entry{}; entry < GetFsEntriesCount(); entry++) {
            NcaFilesystemInfo fsInfo{nca, header.entries[entry], entry};
            const auto backing{fsInfo.MountEncryptedFile(*this)};

            const auto unaligned{backing->GetBytes(50)};
            u32 magic{};
            std::memcpy(&magic, &unaligned[0], sizeof(magic));
            if (fsInfo.isPartition && magic)
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
        std::optional<crypto::Key128> key;

        if (crypto::KeyIsEmpty(header.rightsId)) {
            const auto keyIndex = [&] -> u64 {
                switch (type) {
                    case EncryptionType::AesCtr:
                        return 2;
                    case EncryptionType::AesXts:
                    default:
                        return {};
                }
            }();
            key = keys.GetKey(header.keyIndexType, GetGenerationKey());
            result = header.encryptedKeyArea[keyIndex];
        } else {
            if (const auto title{keys.GetTitle(header.rightsId)})
                result = *title;

            assert(header.keyIndexType == crypto::Application);
            key = keys.GetKey(crypto::Titlekek, GetGenerationKey());
        }
        if (!key)
            throw std::runtime_error("Title/Area key missing");

        crypto::AesStorage cipher(MBEDTLS_CIPHER_AES_128_ECB, key->data(), key->size());
        cipher.Decrypt(result.data(), result.data(), sizeof(result));
        return result;
    }
}
