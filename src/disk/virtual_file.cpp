#include <disk/virtual_file.h>
namespace peachnx::disk {
    VirtualFile::VirtualFile(const std::filesystem::path& filename, const DiskAccess diskAccess, const bool isPresent) :
        privilege(diskAccess),
        diskPath(filename) {
        if (!isPresent)
            return;

        size = file_size(diskPath);
        if (!is_regular_file(diskPath)) {
            throw std::runtime_error("This is not a regular or physical file");
        }
        flushed = true;
    }
    u64 VirtualFile::GetSize() const {
        return size;
    }
}
