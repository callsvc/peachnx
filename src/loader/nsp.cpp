#include <ranges>

#include <crypto/ticket.h>
#include <loader/nsp.h>
namespace peachnx::loader {

    NSP::NSP(const disk::VirtFilePtr& nsp, const u64 titleId, const u64 programIndex) :
        pfs(std::make_unique<disk::PartitionFilesystem>(nsp)),
        source(nsp), program(titleId), index(programIndex) {

        const auto& files{pfs->GetAllFiles()};
        for (const auto& filename : std::views::keys(files) ) {
            if (!filename.ends_with(".tik")) {
                continue;
            }
            [[maybe_unused]] crypto::Ticket ticket{files.at(filename)};
        }
    }

    bool NSP::IsValidNsp() const {
        return pfs->header.entryCount != 0;
    }
}
