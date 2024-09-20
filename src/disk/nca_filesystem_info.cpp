#include <cassert>
#include <disk/nca.h>
#include <disk/nca_filesystem_info.h>
namespace peachnx::disk {
    NcaFilesystemInfo::NcaFilesystemInfo(const VirtFilePtr& nca, const u32 index) :
        section(index), offset(sizeof(NcaHeader) + sizeof(FsEntry) * index) {
        header = nca->Read<NsaFsHeader>(offset);

        encrypted = header.version != FsHeaderVersion;
    }
    VirtFilePtr NcaFilesystemInfo::OpenEncryptedStorage(std::optional<crypto::AesStorage>& storage) {
        if (encrypted && !storage) {
            throw std::runtime_error("No valid XTS context provided");
        }
        if (!encrypted) {
            return {};
        }
        storage->DecryptXts<NsaFsHeader>(header, 2 + section, 0x200);
        assert(header.version == FsHeaderVersion);

        return {};
    }
}
    