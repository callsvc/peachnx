#include <sys_fs/virtual_file.h>
namespace Peachnx::SysFs {
    VirtualFile::VirtualFile(const std::filesystem::path& filename, const DiskAccess access, const bool tangible, const u64 virtOff, const u64 virtSize) :
        privilege(access), diskPath(filename) {

        if (!tangible) {
            if (privilege == DiskAccess::Read)
                readOffset = virtOff;
            else if (privilege == DiskAccess::Write)
                writeOffset = virtOff;
            size = virtSize;
            return;
        }
        size = file_size(diskPath);
        if (!is_regular_file(diskPath)) {
            throw std::runtime_error("This is not a regular or physical file");
        }
        flushed = true;
    }
    std::vector<u8> VirtualFile::GetBytes(const u64 size, const u64 offset) {
        std::vector<u8> content(size);
        if (GetBytesImpl(content, offset) != size) {
            throw std::runtime_error("Unable to read the required number of bytes");
        }
        return content;
    }

    u64 VirtualFile::GetBytesImpl(std::vector<u8>& output, const u64 offset) {
        return ReadImpl(std::span(output), offset);
    }
}
