#include <ranges>
#include <print>

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
        std::array<u8, 0x20> portion; // SHA-256 hash of the first (size of hashed region) bytes of filedata
    };
#pragma pack(pop)

    PartitionFilesystem::PartitionFilesystem(const VirtFilePtr& pfs, const bool displayContent) {
        header = pfs->Read<PfsHeader>();

        if (header.magic == MakeMagic<u32>("HFS0"))
            isHfs = checked = true;
        else if (header.magic == MakeMagic<u32>("PFS0"))
            checked = true;

        const u64 entrySize{isHfs ? sizeof(HfsEntry) : sizeof(PfsEntry)};
        // Size of the region that does not represent any specific file content (Metadata region)
        const u64 metaSize{sizeof(PfsHeader) + (header.entryCount * entrySize) + header.strOff};
        if (!metaSize || metaSize > std::numeric_limits<u32>::max()) {
            if (checked)
                return;
            throw std::runtime_error("Possible data corruption detected");
        }

        auto content{pfs->GetBytes(metaSize)};
        constexpr auto entriesOffset{sizeof(header)};
        const auto stringTableOffset{entriesOffset + header.entryCount * entrySize};
        const auto contentOffset{stringTableOffset + header.strOff};

        std::stringstream outBuffer;
        outBuffer << "Entries in this PFS\n";
        for (u32 entry{}; entry < header.entryCount; entry++) {
            PfsEntryRecord record;
            std::memcpy(&record, &content[entriesOffset + entry * entrySize], sizeof(record));

            std::string filename{reinterpret_cast<char*>(&content[stringTableOffset + record.strOff])};
            outBuffer << std::format("New file named {} found\n", filename);

            auto reference{std::make_shared<OffsetFile>(pfs, contentOffset + record.offset, record.size, filename)};

            pfsFiles.emplace(filename, reference);
        }

        if (displayContent)
            std::print("{}", outBuffer.str());
    }

    bool PartitionFilesystem::ContainsFile(const std::string_view& filename) {
        const auto& files{GetAllFiles()};
        return files.contains(std::string(filename));
    }
    VirtFilePtr PartitionFilesystem::OpenFile(const std::string_view& filename) {
        for (const auto& [name, file] : pfsFiles) {
            if (name == filename)
                if (file->GetSize())
                    return file;
        }
        return {};
    }
}
