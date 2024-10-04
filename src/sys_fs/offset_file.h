#pragma once
#include <sys_fs/virtual_file.h>

namespace Peachnx::SysFs {
    class OffsetFile final : public VirtualFile {
    public:
        explicit OffsetFile(const std::shared_ptr<VirtualFile>& parent, u64 offset, u64 size, const std::filesystem::path& filename);
    private:
        std::shared_ptr<VirtualFile> backing;
        u64 ReadImpl(const std::span<u8>& output, u64 offset) override;
    };
}