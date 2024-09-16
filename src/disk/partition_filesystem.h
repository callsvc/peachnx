#pragma once
#include <boost/unordered_map.hpp>
#include <disk/virtual_types.h>
namespace peachnx::disk {
    // https://switchbrew.org/wiki/XCI#PartitionFs
    // https://switchbrew.org/wiki/NCA#PFS0
#pragma pack(push, 1)
    struct PfsHeader {
        u32 magic;
        u32 entryCount;
        u32 stringTableSize;
        u32 pad0;
    };
    struct PfsEntryRecord {
        u64 offset;
        u64 size;
        u32 strOff;
    };
#pragma pack(pop)

    class PartitionFilesystem {
    public:
        explicit PartitionFilesystem(const VirtFilePtr& pfs);

        auto& GetAllFiles() {
            if (header.entryCount < pfsFiles.size()) {
            }
            return pfsFiles;
        }

        PfsHeader header;
        bool isHfs;
        bool checked;
    private:
        // Note: The Boost unordered_map library version shows a 20% increase in operations per second after version 1.80
        boost::unordered_map<std::string, OffsetFilePtr> pfsFiles;

        struct PfsEntry;
        // Use this structure when "isHfs" is true
        struct HfsEntry;
    };
}