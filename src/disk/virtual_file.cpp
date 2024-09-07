#include <disk/virtual_file.h>
namespace peachnx::disk {
    VirtualFile::VirtualFile(const std::filesystem::path& filename, const DiskAccess diskAccess, const bool isPresent) :
        privilege(diskAccess),
        diskName(filename) {
        if (!isPresent)
            return;

        size = file_size(diskName);
        if (!is_regular_file(diskName)) {
            throw std::runtime_error("This is not a regular or physical file");
        }
        flushed = true;
    }
    u64 VirtualFile::GetSize() const {
        return size;
    }
}
