#include <loader/nsp.h>

namespace peachnx::loader {

    NSP::NSP(const disk::VirtFilePtr& nsp, const u64 titleId, const u64 programIndex) :
        pfs(std::make_unique<disk::PartitionFilesystem>(nsp)),
        source(nsp), program(titleId), index(programIndex) {

    }

    bool NSP::IsValidNsp() const {
        return pfs->pfs0.entry;
    }
}
