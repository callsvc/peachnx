#pragma once

#include <disk/virtual_types.h>
namespace peachnx::disk {
    struct NsaFsHeader {
        u32 version; // (always 2)
    };

    class NcaFilesystemInfo {
    public:
        NcaFilesystemInfo(const VirtFilePtr& nca, u32 index);
    private:
        NsaFsHeader header;
    };
}