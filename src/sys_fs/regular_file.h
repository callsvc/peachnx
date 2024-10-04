#pragma once

#include <sys_fs/virtual_file.h>
namespace Peachnx::SysFs {
    class RegularFile final : public VirtualFile {
    public:
        explicit RegularFile(const std::filesystem::path& filename, DiskAccess access = DiskAccess::Read);

        u64 GetSize();
        void RequestADifferentFileMode(DiskAccess access);
    private:
        u64 ReadImpl(const std::span<u8>& output, u64 offset) override;

        i32 descriptor;
    };
}