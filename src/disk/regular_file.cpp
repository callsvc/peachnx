#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <disk/regular_file.h>
namespace peachnx::disk {
    auto mkAccessFlags = [] (const DiskAccess access) -> i32 {
        if (access == DiskAccess::Rw)
            return O_RDWR;
        if (access == DiskAccess::Read)
            return O_RDONLY;
        return {};
    };

    RegularFile::RegularFile(const std::filesystem::path& filename, const DiskAccess access) :
        VirtualFile(filename, access, true) {

        descriptor = open64(filename.c_str(), mkAccessFlags(access));
        if (descriptor < 3)
            throw std::runtime_error("Could not open file");
    }
    u64 RegularFile::GetSize() {
        struct stat64 status{};
        if (stat64(diskPath.c_str(), &status)) {
            throw std::runtime_error("The file exists but is not accessible");
        }
        size = status.st_size;
        return size;
    }

    u64 RegularFile::ReadImpl(const std::span<u8>& output, const u64 offset) {
        if (privilege == DiskAccess::Write) {
            throw std::runtime_error("Attempt to read from a write-only file");
        }

        decltype(size) blkOffset{};
        do {
            const auto missing{output.size() - blkOffset};
            const u64 stride{missing < 4096 ? missing : 4096};
            const auto effective{readOffset + offset + blkOffset};

            const auto result{pread64(descriptor, &output[blkOffset], stride, effective)};
            if (result < 0)
                throw std::runtime_error("Could not read from file");

            blkOffset += result;
        } while (blkOffset < output.size());

        return blkOffset;
    }

    void RegularFile::RequestADifferentFileMode(const DiskAccess access) {
        if (access == privilege)
            throw std::runtime_error("The file access type has already been established");

        if (fcntl(descriptor, F_SETFD, mkAccessFlags(access)))
            throw std::runtime_error("Failed to change the file permissions");

        privilege = access;
        if (access == DiskAccess::Read)
            flushed = true;
    }
}
