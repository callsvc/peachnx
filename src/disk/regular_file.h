#pragma once

#include <disk/virtual_file.h>
namespace peachnx::disk {
    class RegularFile final : public VirtualFile {
    public:
        explicit RegularFile(const std::filesystem::path& filename, DiskAccess access);

        u64 GetSize();
        void RequestADifferentFileMode(DiskAccess access);
    private:
        u64 ReadImpl(const std::span<u8>& output, u64 offset) override;

        i32 descriptor;
    };
}