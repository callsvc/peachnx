#include <ranges>

#include <crypto/ticket.h>
#include <loader/nsp.h>
#include <disk/nca.h>
namespace peachnx::loader {

    NSP::NSP(crypto::KeysDb& keysDb, const disk::VirtFilePtr& nsp, const u64 titleId, const u64 programIndex) :
        pfs(std::make_unique<disk::PartitionFilesystem>(nsp, true)), keys(keysDb), program(titleId), index(programIndex) {

        const auto& files{pfs->GetAllFiles()};
        for (const auto& filename: std::views::keys(files)) {
            if (!filename.ends_with(".tik")) {
                continue;
            }
            crypto::Ticket ticket{files.at(filename)};
            keysDb.AddTicket(ticket);
            crypto::Key128 value;
            std::memcpy(&value, &ticket.titleKeyBlock, sizeof(value));
            if (!crypto::KeyIsEmpty(value)) {
                keysDb.AddTitleKey(ticket.rightsId, value);
            }
        }

        ReadContent(files);
    }
    void NSP::ReadContent(const boost::unordered_map<std::string, std::shared_ptr<disk::OffsetFile>>& files) {
        for (const auto& [filename, content]: files) {
            if (!filename.ends_with(".cnmt.nca") && !filename.ends_with(".nca"))
                continue;

            auto nca{std::make_unique<disk::NCA>(*keys, content)};
        }
    }

    NSP::NSP(const disk::VirtFilePtr& nsp) :
        pfs(std::make_unique<disk::PartitionFilesystem>(nsp)), program(0), index(0) {}

    bool NSP::IsValidNsp() const {
        return pfs->header.entryCount != 0;
    }
}
