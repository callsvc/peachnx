#include <cassert>

#include <boost/endian/conversion.hpp>

#include <disk/nca.h>
#include <disk/encrypted_ranged_file.h>
#include <disk/nca_filesystem_info.h>

namespace peachnx::disk {
    NcaFilesystemInfo::NcaFilesystemInfo(const VirtFilePtr& nca, const FsEntry& fsInfo, const u32 index) :
        parent(nca), section(index), offset(sizeof(NcaHeader) + sizeof(FsEntry) * index), entry(fsInfo) {
        header = nca->Read<NsaFsHeader>(offset);
        offset = entry.startSector * 0x200;
        count = (entry.endSector - entry.startSector) * 0x200;

        encrypted = header.version != fsHeaderVersion;
    }
    VirtFilePtr NcaFilesystemInfo::MountEncryptedFile(NCA& nca) {
        nca.cipher->DecryptXts<NsaFsHeader>(header, 2 + section, 0x200);

        isPartition = header.type == NsaFsHeader::PartitionFs;
        const auto mbedType = [&] {
            if (header.encryptionType == EncryptionType::None) {
                return MBEDTLS_CIPHER_NONE;
            }
            if (header.encryptionType == EncryptionType::AesXts)
                return MBEDTLS_CIPHER_AES_128_XTS;
            if (header.encryptionType == EncryptionType::AesCtr)
                return MBEDTLS_CIPHER_AES_128_CTR;

            throw std::runtime_error("Unsupported encryption type for the current content");
        }();

        if (mbedType != MBEDTLS_CIPHER_NONE)
            assert(encrypted && header.encryptionType != EncryptionType::None);

        auto containedBacking = [&] -> VirtFilePtr {
            auto secure{header.nonce};

            boost::endian::endian_reverse_inplace(secure);
            EncryptContext encrypted(mbedType, nca.ReadExternalKey(header.encryptionType), entry.startSector);

            switch (header.encryptionType) {
                case EncryptionType::None:
                    return std::make_shared<OffsetFile>(parent, offset, count, GetFileName());
                case EncryptionType::AesCtr:
                    std::memcpy(&encrypted.nonce[0], &secure, sizeof(u64));
                case EncryptionType::AesXts:
                    return std::make_shared<EncryptedRangedFile>(parent, encrypted, offset, count, GetFileName());
                default:
                    return nullptr;
            }
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
    