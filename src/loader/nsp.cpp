#include <ranges>
#include <print>

#include <crypto/ticket.h>
#include <loader/nsp.h>

#include <generic.h>
namespace Peachnx::Loader {

    NSP::NSP(const std::shared_ptr<Crypto::KeysDb>& kdb, const SysFs::VirtFilePtr& nsp, const u64 titleId, const u64 programIndex) :
        pfs(std::make_unique<SysFs::PartitionFilesystem>(nsp, true)), keys(kdb), program(titleId), index(programIndex) {

        const auto& files{pfs->GetAllFiles()};
        for (const auto& filename: std::views::keys(files)) {
            if (!filename.ends_with(".tik")) {
                continue;
            }
            Crypto::Ticket ticket{files.at(filename)};
            kdb->AddTicket(ticket);
            Crypto::Key128 value;
            std::memcpy(&value, &ticket.titleKeyBlock, sizeof(value));
            if (!Crypto::KeyIsEmpty(value)) {
                kdb->AddTitleKey(ticket.rightsId, value);
            }
        }

        ReadContent(files);
    }
    void NSP::ReadContent(const boost::unordered_map<std::string, SysFs::VirtFilePtr>& files) {
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
                const auto title{nca->GetProgramId()};
                contents.emplace(title, std::move(nca));
                continue;
            }
            const auto& section0{nca->GetDirectories().front()};

            // We are searching for the section that contains only metadata for NCA packages, "NCA-type0"
            for (const auto& [filename, inner] : section0->GetAllFiles()) {
                if (!filename.ends_with(".cnmt"))
                    continue;

                cnmts.emplace_back(inner);
                contents.emplace(cnmts.back().GetTitleId(), std::move(nca));

                // ReSharper disable once CppUseStructuredBinding
                for (const auto& rec : cnmts.back().records) {
                    auto nextFile{std::format("{}.nca", ByteArrayToString(rec.contentId))};

                    if (!pfs->ContainsFile(nextFile)) {
                        std::print("NCA named {}, listed in the metadata entry, does not exist\n", nextFile);
                        continue;
                    }
                    auto nextNca{std::make_unique<NCA>(keys, pfs->OpenFile(nextFile))};
                    auto title{nextNca->GetProgramId()};
                    if (nextNca->type == ContentType::Program) {
                        contents.emplace(title & 0xfffffffffffff000, std::move(nextNca));
                    } else {
                        contents.emplace(title, std::move(nextNca));
                    }
                }
            }
        }
    }

    bool NSP::CheckIntegrity() const {
        if (!contents.empty()) {
            return !pfs->GetAllFiles().empty();
        }
        return !pfs->GetAllFiles().empty();
    }
    std::vector<u64> NSP::GetProgramIds() {
        std::vector<u64> result;
        result.reserve(contents.size());
        for (const auto& titleId : std::ranges::views::keys(contents)) {
            result.emplace_back(titleId);
        }
        return result;
    }

    NSP::NSP(const SysFs::VirtFilePtr& nsp) :
        pfs(std::make_unique<SysFs::PartitionFilesystem>(nsp)), program(), index() {}
}
