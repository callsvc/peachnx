#include <disk/offset_file.h>

namespace peachnx::disk {

    OffsetFile::OffsetFile(const std::shared_ptr<VirtualFile>& parent, const u64 offset, const u64 size, const std::filesystem::path& filename) :
        VirtualFile(filename, DiskAccess::Read, false, offset, size), backing(parent) {}

    u64 OffsetFile::ReadImpl(const std::span<u8>& output, const u64 offset) {
        std::span buffer{output};
        if (output.size() > size - offset) {
            buffer = output.subspan(0, size - offset);
        }
        const auto result{backing->Read(buffer, rd + offset)};
        if (result != buffer.size()) {
            throw std::runtime_error("Read failed");
        }
        return result;
    }
}
