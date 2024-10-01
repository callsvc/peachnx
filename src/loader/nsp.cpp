#include <ranges>
#include <print>

#include <crypto/ticket.h>
#include <loader/nsp.h>

#include <generic.h>
namespace peachnx::loader {

    NSP::NSP(const std::shared_ptr<crypto::KeysDb>& kdb, const disk::VirtFilePtr& nsp, const u64 titleId, const u64 programIndex) :
        pfs(std::make_unique<disk::PartitionFilesystem>(nsp, true)), keys(kdb), program(titleId), index(programIndex) {

        const auto& files{pfs->GetAllFiles()};
        for (const auto& filename: std::views::keys(files)) {
            if (!filename.ends_with(".tik")) {
                continue;
            }
            crypto::Ticket ticket{files.at(filename)};
            kdb->AddTicket(ticket);
            crypto::Key128 value;
            std::memcpy(&value, &ticket.titleKeyBlock, sizeof(value));
            if (!crypto::KeyIsEmpty(value)) {
                kdb->AddTitleKey(ticket.rightsId, value);
            }
        }

        ReadContent(files);
    }
    void NSP::ReadContent(const boost::unordered_map<std::string, disk::VirtFilePtr>& files) {
        for (const auto& [filename, content]: files) {
            if (!filename.ends_with(".cnmt.nca") &&
                !filename.ends_with(".nca"))
                continue;

            auto nca{std::make_unique<NCA>(keys, content)};
#define ENB_CHECK_FOR_INTEGRITY 0

#if ENB_CHECK_FOR_INTEGRITY
            nca->VerifyNcaIntegrity();
#endif

            if (nca->GetDirectories().empty()) {
                contents.emplace_back(std::move(nca));
                continue;
            }

            const auto& section0{nca->GetDirectories().front()};
            // We are searching for the section that contains only metadata for NCA packages, "NCA-type0"
            for (const auto& [filename, inner] : section0->GetAllFiles()) {
                if (!filename.ends_with(".cnmt"))
                    continue;
                cnmts.emplace_back(inner);
                indexedNca.emplace(cnmts.back().GetTitleId(), contents.size());

                // ReSharper disable once CppUseStructuredBinding
                for (const auto& rec : cnmts.back().records) {
                    auto nextNca{std::format("{}.nca", ByteArrayToString(rec.contentId))};

                    if (!pfs->ContainsFile(nextNca)) {
                        std::print("NCA named {}, listed in the metadata entry, does not exist\n", nextNca);
                    }
                }
            }

            contents.emplace_back(std::move(nca));
        }
    }

    NSP::NSP(const disk::VirtFilePtr& nsp) :
        pfs(std::make_unique<disk::PartitionFilesystem>(nsp)), program(), index() {}

    ApplicationType NSP::GetTypeFromFile(const disk::VirtFilePtr& probFile) {
        if (const u32 magic = probFile->Read<u32>()) {
            if (magic == MakeMagic<u32>("NSP"))
                return ApplicationType::NSP;
        }
        return ApplicationType::Unrecognized;
    }

    bool NSP::IsLoaded() const {
        if (!contents.empty()) {
            return !pfs->GetAllFiles().empty();
        }
        return !pfs->GetAllFiles().empty();
    }
}
