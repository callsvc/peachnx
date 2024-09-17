#include <ranges>

#include <crypto/ticket.h>
#include <loader/nsp.h>
namespace peachnx::loader {

    NSP::NSP(crypto::KeysDb& KeysDb, const disk::VirtFilePtr& nsp, const u64 titleId, const u64 programIndex) :
        pfs(std::make_unique<disk::PartitionFilesystem>(nsp)), source(nsp), program(titleId), index(programIndex) {

        const auto& files{pfs->GetAllFiles()};
        for (const auto& filename: std::views::keys(files)) {
            if (!filename.ends_with(".tik")) {
                continue;
            }
            crypto::Ticket ticket{files.at(filename)};
            KeysDb.AddTicket(ticket);
            crypto::Key128 value;
            if (!KeysDb.headerKey) {
                std::memcpy(&value, &ticket.titleKeyBlock, sizeof(value));
                KeysDb.AddTitleKey(ticket.rightsId, value);
            }
        }
    }
    NSP::NSP(const disk::VirtFilePtr& nsp) :
        pfs(std::make_unique<disk::PartitionFilesystem>(nsp)), program(0), index(0) {}

    bool NSP::IsValidNsp() const {
        return pfs->header.entryCount != 0;
    }
}
