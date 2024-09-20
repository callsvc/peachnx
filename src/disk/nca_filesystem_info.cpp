#include <cassert>
#include <disk/nca.h>
#include <disk/encrypted_ranged_file.h>
#include <disk/nca_filesystem_info.h>

namespace peachnx::disk {
    NcaFilesystemInfo::NcaFilesystemInfo(const VirtFilePtr& nca, const FsEntry& detail, const u32 index) :
        parent(nca), section(index), offset(sizeof(NcaHeader) + sizeof(FsEntry) * index), entry(detail) {
        header = nca->Read<NsaFsHeader>(offset);
        offset = entry.startSector * 0x200;
        count = (entry.endSector - entry.startSector) * 0x200;

        encrypted = header.version != FsHeaderVersion;
    }
    VirtFilePtr NcaFilesystemInfo::OpenEncryptedStorage(const crypto::KeysDb& keysDb, std::optional<crypto::AesStorage>& storage) {
        if (encrypted && !storage) {
            throw std::runtime_error("No valid XTS context provided");
        }
        if (!encrypted) {
            return std::make_shared<OffsetFile>(parent, offset, count, GetEntryName());
        }
        storage->DecryptXts<NsaFsHeader>(header, 2 + section, 0x200);
        assert(header.version == FsHeaderVersion);
        isPartition = header.type == NsaFsHeader::PartitionFs;

        const auto mbedType = [&] {
            if (header.encryptionType == EncryptionType::AesXts)
                return MBEDTLS_CIPHER_AES_128_XTS;
            return MBEDTLS_CIPHER_NULL;
        }();

        if (mbedType == MBEDTLS_CIPHER_NULL)
            throw std::runtime_error("Unsupported encryption type for the current content");

        const auto headerKey{*keysDb.headerKey};
        EncryptContext encrypted{mbedType, headerKey, entry.startSector};

        auto containedBacking = [&] -> VirtFilePtr {
            switch (header.encryptionType) {
                case EncryptionType::AesXts:
                    return std::make_shared<EncryptedRangedFile>(parent, encrypted, offset, count, GetEntryName());
                default: {}
            }
            return {};
        }();
        return containedBacking;
    }
    std::string NcaFilesystemInfo::GetEntryName() const {
        std::string filename;
        filename.reserve(64);
        if (header.type == NsaFsHeader::PartitionFs) {
            filename.append("directory.");
        } else {
            filename.append("file.");
        }
        filename.append(std::format("{:08x}.{:08x}", offset, count));
        const auto superDisk{parent->GetDiskPath()};
        if (!superDisk.empty()) {
            const auto suffix{superDisk.filename().string()};
            filename.append(std::format(".{}", suffix.substr(0, 8)));
        }

        return filename;
    }
}
    