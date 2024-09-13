#pragma once

#include <disk/virtual_types.h>
#include <disk/partition_filesystem.h>
namespace peachnx::loader {
    class NSP {
    public:
        explicit NSP(const disk::VirtFilePtr& nsp, u64 titleId = 0, u64 programIndex = 0);

        bool IsValidNsp() const;
        std::unique_ptr<disk::PartitionFilesystem> pfs;
    private:
        disk::VirtFilePtr source;
        u64 program;
        u64 index;
    };
}