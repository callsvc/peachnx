#include <disk/virtual_file.h>
namespace peachnx::disk {
    VirtualFile::VirtualFile(const std::filesystem::path& filename, const DiskAccess diskAccess, const bool isPresent) :
        privilege(diskAccess), diskPath(filename) {
        if (!isPresent)
            return;

        size = file_size(diskPath);
        if (!is_regular_file(diskPath)) {
            throw std::runtime_error("This is not a regular or physical file");
        }
        flushed = true;
    }
    std::vector<u8> VirtualFile::GetBytes(const u64 size, const u64 offset) {
        std::vector<u8> content(size);
        GetBytesImpl(content, offset);
        return content;
    }

    void VirtualFile::GetBytesImpl(std::vector<u8>& output, const u64 offset) {
        ReadImpl(std::span(output), offset);
    }
}
