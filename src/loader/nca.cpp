#include <cassert>
#include <ranges>

#include <boost/algorithm/string.hpp>
#include <mbedtls/sha256.h>

#include <loader/nca.h>
#include <generic.h>
namespace peachnx::loader {
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


    NCA::NCA(const std::shared_ptr<crypto::KeysDb>& kdb, const disk::VirtFilePtr& content) : keys(kdb), nca(content) {
        if (!content)
            return;
        header = content->Read<NcaHeader>();

        u32 archive;
        if ((archive = CheckContentArchiveMagic(header.magic)) == 0) {
            cipher.emplace(MBEDTLS_CIPHER_AES_128_XTS, keys->headerKey->data(), keys->headerKey->size());
        }
        if (archive != MakeMagic<u32>("NCA3")) {
            if (!keys->headerKey)
                throw std::runtime_error("Content key inaccessible");

            cipher->DecryptXts<NcaHeader>(header, 0, 0x200);
            archive == CheckContentArchiveMagic(header.magic);

            if (header.sdkAddonVersion < sdkMinimumVersion)
                throw std::runtime_error("SDK addon version too small");
        }
        // Checking if the user-provided keys meet the requirements to proceed
        const auto keyIndex{header.keyIndexType};
        if (!keys->GetKey(crypto::Application, GetGenerationKey(), keyIndex)) {
            throw std::runtime_error("Key area not found");
        }

        assert(content->GetSize() == header.size);
        type = header.contentType;

        ReadContent(content);
    }
    ApplicationType NCA::GetTypeFromFile(const disk::VirtFilePtr& probFile) {
        if ([[maybe_unused]] const auto magic = probFile->Read<u32>())
            return ApplicationType::NCA;
        return ApplicationType::Unrecognized;
    }
    void NCA::ReadContent(const disk::VirtFilePtr& content) {
        for (u32 entry{}; entry < GetFsEntriesCount(); entry++) {
            const auto& fsEntry{header.entries[entry]};
            const auto& hashOverHeader{header.fsHeaderHashes[entry]};

            NcaFilesystemInfo fsInfo{content, fsEntry, entry};
            const auto backing{fsInfo.MountEncryptedFile(hashOverHeader, *this)};

            const auto magic{backing->Read<u32>()};
            if (magic == MakeMagic<u32>("PFS0")) {
                ReadPartitionFs(backing);
            } else {
                ReadRomFs(backing);
            }
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
            key = keys->GetKey(header.keyIndexType, GetGenerationKey());
            result = header.encryptedKeyArea[keyIndex];
        } else {
            if (const auto title{keys->GetTitle(header.rightsId)})
                result = *title;

            assert(header.keyIndexType == crypto::Application);
            key = keys->GetKey(crypto::Titlekek, GetGenerationKey());
        }
        if (!key)
            throw std::runtime_error("Title/Area key missing");

        crypto::AesStorage cipher(MBEDTLS_CIPHER_AES_128_ECB, key->data(), key->size());
        cipher.Decrypt(result.data(), result.data(), sizeof(result));
        return result;
    }

    void NCA::ReadPartitionFs(const disk::VirtFilePtr& content) {
        dirs.emplace_back(std::make_shared<disk::PartitionFilesystem>(content, true));

        const auto& files{dirs.back()->GetAllFiles()};

        if (header.contentType == ContentType::Meta) {
            cnmt = dirs.back();
            return;
        }
        assert(header.contentType == ContentType::Program);

        if (files.contains("main")) {
            assert(files.contains("main.npdm"));
            exeFs = dirs.back();
        }
        if (files.contains("NintendoLogo.png")) {
            // Nintendo logo displayed in the top-left of the screen
            assert(files.contains("StartupMovie.gif"));
            logo = dirs.back();
        }
    }
    void NCA::ReadRomFs(const disk::VirtFilePtr& content) {
        files.emplace_back(content);
        romFs = files.back();
    }


    bool NCA::VerifyNcaIntegrity() {
        std::vector<std::string> ncaParts;
        split(ncaParts, nca->GetDiskPath().string(), boost::is_any_of("."));

        constexpr auto ncaHashSize{32};

        if (ncaParts.back() != "nca")
            return {};

        std::vector<u8> content(4 * 1024 * 1024);
        mbedtls_sha256_context context;
        mbedtls_sha256_init(&context);
        mbedtls_sha256_starts(&context, 0);

        const auto expected{StringToByteArray<ncaHashSize / 2>(ncaParts.front())};
        std::array<u8, ncaHashSize> result;
        const auto totalSize{nca->GetSize()};

        for (u64 offsetCount{}; offsetCount < totalSize;) {
            const auto stride{std::min(content.size(), totalSize - offsetCount)};
            if (stride < content.size())
                content.resize(stride);

            nca->Read(content, offsetCount);
            offsetCount += stride;

            mbedtls_sha256_update(&context, content.data(), stride);
        }
        mbedtls_sha256_finish(&context, result.data());
        mbedtls_sha256_free(&context);

        verified = std::memcmp(expected.data(), result.data(), sizeof(expected)) == 0;
        return verified;
    }
}
