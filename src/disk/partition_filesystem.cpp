#include <disk/partition_filesystem.h>

namespace peachnx::disk {
    struct PartitionFilesystem::PfsEntry {

    };
    struct PartitionFilesystem::HfsEntry {
    };

    PartitionFilesystem::PartitionFilesystem(const VirtFilePtr& pfs) : isHfs(), checked() {
        pfs0 = pfs->Read<PfsHeader>();

        if (pfs0.magic == MakeMagic<u32>("HFS0"))
            isHfs = checked = true;
        else if (pfs0.magic == MakeMagic<u32>("PFS0"))
            checked = true;

        if (!checked) {
            throw std::invalid_argument("Invalid partition filesystem");
        }

        const u64 size{isHfs ? sizeof(HfsEntry) : sizeof(PfsEntry)};
        // Size of the region that does not represent any specific file content (Metadata region)
        const u64 metadata{sizeof(PfsHeader) + (pfs0.entry * size) + pfs0.stringTableSize};
        if (!metadata || metadata > std::numeric_limits<u32>::max()) {
            throw std::runtime_error("Possible data corruption detected");
        }
        [[maybe_unused]] auto pfsMetadata{pfs->GetBytes(metadata)};

    }
}
