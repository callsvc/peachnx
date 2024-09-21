#include <cassert>

#include <boost/endian/conversion.hpp>

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
    VirtFilePtr NcaFilesystemInfo::MountEncryptedStorage(const crypto::KeysDb& keysDb, std::optional<crypto::AesStorage>& storage) {
        if (encrypted && !storage) {
            throw std::runtime_error("No valid XTS context provided");
        }
        if (!encrypted) {
            return std::make_shared<OffsetFile>(parent, offset, count, GetFileName());
        }
        storage->DecryptXts<NsaFsHeader>(header, 2 + section, 0x200);
        assert(header.version == FsHeaderVersion);
        isPartition = header.type == NsaFsHeader::PartitionFs;

        const auto mbedType = [&] {
            if (header.encryptionType == EncryptionType::AesXts)
                return MBEDTLS_CIPHER_AES_128_XTS;
            if (header.encryptionType == EncryptionType::AesCtr)
                return MBEDTLS_CIPHER_AES_128_CTR;

            throw std::runtime_error("Unsupported encryption type for the current content");
        }();

        EncryptContext fileInfo{mbedType, *keysDb.headerKey, entry.startSector};
        auto containedBacking = [&] -> VirtFilePtr {
            auto secure{header.secureValue};
            auto generation{header.generation};

            boost::endian::endian_reverse_inplace(secure);
            boost::endian::endian_reverse_inplace(generation);
            if (mbedType == MBEDTLS_CIPHER_AES_128_CTR) {
                std::memcpy(&fileInfo.nonce[0], &secure, sizeof(u32));
                std::memcpy(&fileInfo.nonce[4], &generation, sizeof(u32));
            }
            return std::make_shared<EncryptedRangedFile>(parent, fileInfo, offset, count, GetFileName());
        }();

        return containedBacking;
    }

    std::string NcaFilesystemInfo::GetFileName() const {
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
    