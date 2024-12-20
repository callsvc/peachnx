#pragma once
#include <boost/unordered_map.hpp>
#include <sys_fs/virtual_types.h>
namespace Peachnx::SysFs {
    // https://switchbrew.org/wiki/XCI#PartitionFs
    // https://switchbrew.org/wiki/NCA#PFS0
#pragma pack(push, 1)
    struct PfsHeader {
        u32 magic;
        u32 entryCount;
        u32 strOff;
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
        explicit PartitionFilesystem(const VirtFilePtr& pfs, bool displayContent = false);

        auto& GetAllFiles() {
            assert(header.entryCount <= pfsFiles.size());
            return pfsFiles;
        }

        bool ContainsFile(const std::string_view& filename);
        VirtFilePtr OpenFile(const std::string_view& filename);

        PfsHeader header;
        bool isHfs{};
        bool checked{};
    private:
        // Note: The Boost unordered_map library version shows a 20% increase in operations per second after version 1.80
        boost::unordered_map<std::string, VirtFilePtr> pfsFiles;

        struct PfsEntry;
        // Use this structure when "isHfs" is true
        struct HfsEntry;
    };
}