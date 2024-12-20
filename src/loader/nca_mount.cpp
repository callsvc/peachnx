#include <cassert>

#include <boost/endian/conversion.hpp>

#include <sys_fs/encrypted_ranged_file.h>
#include <crypto/checksum.h>

#include <loader/nca_mount.h>
#include <loader/nca.h>
namespace Peachnx::Loader {
    NcaMount::NcaMount(const SysFs::VirtFilePtr& nca, const FsEntry& fsInfo, const u32 index) : parent(nca), section(index), entry(fsInfo) {
        const auto offset{(sizeof(NcaHeader) + sizeof(FsHeader) * index)};
        header = nca->Read<FsHeader>(offset);
        cfs = ContentFsInfo(entry);

        encrypted = header.version != fsHeaderVersion;
    }
    SysFs::VirtFilePtr NcaMount::MountEncryptedFile(const std::array<u8, 0x20>& value, NCA& nca) {
        nca.cipher->DecryptXts<FsHeader>(header, 2 + section, 0x200);

        Crypto::Checksum expected(value);
        assert(expected.MatchWith(header));

        if (header.encryptionType != EncryptionType::None)
            assert(encrypted);

        auto containedBacking = [&] -> SysFs::VirtFilePtr {
            auto secure{header.nonce};
            boost::endian::endian_reverse_inplace(secure);

            FixupOffsetAndSize();

            SysFs::EncryptContext encrypted(MBEDTLS_CIPHER_NONE, nca.ReadExternalKey(header.encryptionType));
            auto CreateEncryptedStorage = [&] (const mbedtls_cipher_type_t type) {
                encrypted.type = type;
                return std::make_shared<SysFs::EncryptedRangedFile>(parent, encrypted, cfs.GetSector(), cfs.offset, cfs.size, GetFileName());
            };

            switch (header.encryptionType) {
                case EncryptionType::None:
                default:
                    return std::make_shared<SysFs::OffsetFile>(parent, cfs.offset, cfs.size, GetFileName());
                case EncryptionType::AesCtr:
                    std::memcpy(&encrypted.nonce, &secure, sizeof(u64));
                    return CreateEncryptedStorage(MBEDTLS_CIPHER_AES_128_CTR);
                case EncryptionType::AesXts:
                    return CreateEncryptedStorage(MBEDTLS_CIPHER_AES_128_XTS);
            }
        }();

        return containedBacking;
    }

    std::string NcaMount::GetFileName() const {
        std::string filename;
        filename.reserve(64);
        if (header.type == FsHeader::PartitionFs) {
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
    void NcaMount::FixupOffsetAndSize() {
        if (header.integrity.magic == MakeMagic<u32>("IVFC")) {
            assert(header.integrity.levelsCount == 7);
            cfs.offset += header.integrity.levels.back().offset;
            cfs.size += header.integrity.levels.back().size;
        } else {
            assert(header.sha256Data.layerCount == 2);
            cfs.offset += header.sha256Data.layers[1].offset;
            cfs.size += header.sha256Data.layers[1].size;
        }
    }
    bool NcaMount::IsPartitionFs() const {
        return header.type == FsHeader::PartitionFs;
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
    