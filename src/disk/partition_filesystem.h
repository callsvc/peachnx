#pragma once
#include <disk/virtual_types.h>
namespace peachnx::disk {
    // https://switchbrew.org/wiki/XCI#PartitionFs
    // https://switchbrew.org/wiki/NCA#PFS0
    struct PfsHeader {
        u32 magic;
        u32 entry;
        u64 stringTableSize;
        u32 pad0;
    };
    class PartitionFilesystem {
    public:
        explicit PartitionFilesystem(const VirtFilePtr& pfs);

        PfsHeader pfs0;
        bool isHfs;
        bool checked;
    private:
        struct PfsEntry;
        // Use this structure when "isHfs" is true
        struct HfsEntry;
    };
}