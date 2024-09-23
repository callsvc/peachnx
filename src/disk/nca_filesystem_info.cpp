#include <cassert>

#include <boost/endian/conversion.hpp>

#include <disk/encrypted_ranged_file.h>
#include <disk/nca.h>
#include <disk/nca_filesystem_info.h>

#include <crypto/checksum.h>

namespace peachnx::disk {
    NcaFilesystemInfo::NcaFilesystemInfo(const VirtFilePtr& nca, const FsEntry& fsInfo, const u32 index) :
        parent(nca), section(index), entry(fsInfo) {
        const auto offset{(sizeof(NcaHeader) + sizeof(NsaFsHeader) * index)};
        header = nca->Read<NsaFsHeader>(offset);
        cfs = ContentFsInfo(entry);

        encrypted = header.version != fsHeaderVersion;
    }
    VirtFilePtr NcaFilesystemInfo::MountEncryptedFile(const std::array<u8, 32>& value, NCA& nca) {
        nca.cipher->DecryptXts<NsaFsHeader>(header, 2 + section, 0x200);

        crypto::Checksum expected(value);
        assert(expected.MatchWith(header));

        isPartition = header.type == NsaFsHeader::PartitionFs;
        if (header.encryptionType != EncryptionType::None)
            assert(encrypted);

        auto containedBacking = [&] -> VirtFilePtr {
            auto secure{header.nonce};
            boost::endian::endian_reverse_inplace(secure);

            if (header.type == NsaFsHeader::PartitionFs) {
                cfs.offset += header.sha256Data.layers.front().size;
                cfs.size -= header.sha256Data.layers.front().size;
            }

            EncryptContext encrypted(MBEDTLS_CIPHER_NONE, nca.ReadExternalKey(header.encryptionType));
            auto CreateEncryptedStorage = [&] (const mbedtls_cipher_type_t type) {
                encrypted.type = type;
                return std::make_shared<EncryptedRangedFile>(parent, encrypted, cfs.GetSector(), cfs.offset, cfs.size, GetFileName());
            };

            switch (header.encryptionType) {
                case EncryptionType::None:
                default:
                    return std::make_shared<OffsetFile>(parent, cfs.offset, cfs.size, GetFileName());
                case EncryptionType::AesCtr:
                    std::memcpy(&encrypted.nonce, &secure, sizeof(u64));
                    return CreateEncryptedStorage(MBEDTLS_CIPHER_AES_128_CTR);
                case EncryptionType::AesXts:
                    return CreateEncryptedStorage(MBEDTLS_CIPHER_AES_128_XTS);
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
        filename.append(std::format("{:08x}.{:08x}", cfs.offset, cfs.size));
        const auto superDisk{parent->GetDiskPath()};
        if (!superDisk.empty()) {
            const auto suffix{superDisk.filename().string()};
            filename.append(std::format(".{}", suffix.substr(0, 8)));
        }

        return filename;
    }

    constexpr auto sectorShift{9};
    ContentFsInfo::ContentFsInfo(const FsEntry& fsInfo) :
        offset(fsInfo.startSector << sectorShift), size(fsInfo.endSector - fsInfo.startSector << sectorShift) {
        assert(offset > 0); assert(size > 0);
    }
    u64 ContentFsInfo::GetSector(const bool isOffset) const {
        return isOffset ? offset >> sectorShift : size >> sectorShift;
    }
}
    