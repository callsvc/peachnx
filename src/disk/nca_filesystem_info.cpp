#include <disk/nca_filesystem_info.h>
#include <disk/nca.h>
namespace peachnx::disk {
    NcaFilesystemInfo::NcaFilesystemInfo(const VirtFilePtr& nca, const u32 index) {
        const auto offset{sizeof(NcaHeader) + sizeof(FsEntry) * index};
        header = nca->Read<NsaFsHeader>(offset);
    }
}
    