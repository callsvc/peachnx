#include <ranges>

#include <disk/partition_filesystem.h>
namespace peachnx::disk {
#pragma pack(push, 1)
    struct PartitionFilesystem::PfsEntry {
        u64 offset;
        u64 size;
        u32 strOffset;
        u32 pad0;
    };
    struct PartitionFilesystem::HfsEntry {
        u64 offset;
        u64 size;
        u32 strOffset;
        u32 hashedRegionSize; // (for HFS0s, this is the size of the pre-filedata portion, for NCAs this is usually 0x200)
        u64 pad0; // Always zero, reserved
        std::array<u8, 0x20> portionHash; // SHA-256 hash of the first (size of hashed region) bytes of filedata
    };
#pragma pack(pop)

    PartitionFilesystem::PartitionFilesystem(const VirtFilePtr& pfs) : isHfs(), checked() {
        pfs0 = pfs->Read<PfsHeader>();

        if (pfs0.magic == MakeMagic<u32>("HFS0"))
            isHfs = checked = true;
        else if (pfs0.magic == MakeMagic<u32>("PFS0"))
            checked = true;

        if (!checked) {
            throw std::invalid_argument("Invalid partition filesystem");
        }

        const u64 entrySize{isHfs ? sizeof(HfsEntry) : sizeof(PfsEntry)};
        // Size of the region that does not represent any specific file content (Metadata region)
        const u64 metadata{sizeof(PfsHeader) + (pfs0.entryCount * entrySize) + pfs0.stringTableSize};
        if (!metadata || metadata > std::numeric_limits<u32>::max()) {
            throw std::runtime_error("Possible data corruption detected");
        }

        auto pfsMetadata{pfs->GetBytes(metadata)};
        constexpr auto entriesOffset{sizeof(pfs0)};
        const auto stringTableOffset{entriesOffset + pfs0.entryCount * entrySize};
        const auto contentOffset{stringTableOffset + pfs0.stringTableSize};

        for (u32 entry{}; entry < pfs0.entryCount; entry++) {
            PfsEntryRecord record;
            std::memcpy(&record, &pfsMetadata[entriesOffset + (entry * entrySize)], sizeof(record));

            std::string filename{reinterpret_cast<char*>(&pfsMetadata[stringTableOffset + record.strOff])};
            auto reference{std::make_shared<OffsetFile>(pfs, contentOffset + record.offset, record.size, filename)};

            pfsFiles.emplace(filename, reference);
        }
    }
}
