#pragma once
#include <filesystem>

#include <types.h>
namespace peachnx::disk {
    enum class DiskAccess {
        Read,
        Write,
        Rw,
        Test // Special case, if we only want to know file information (size, creation date...)
    };
    class VirtualFile {
    public:
        explicit VirtualFile(const std::filesystem::path& filename = {}, DiskAccess diskAccess = DiskAccess::Read, bool isPresent = false);
        virtual ~VirtualFile() = default;

        template <typename T> requires (std::is_trivially_copyable_v<T>)
        T Read() {
            T copyable;
            ReadImpl({reinterpret_cast<u8*>(&copyable), sizeof(copyable)}, wr);
            return copyable;
        }
        [[nodiscard]] auto GetFilePrivilege() const {
            return privilege;
        }
        [[nodiscard]] auto GetDiskPath() const {
            return diskPath;
        }

        [[nodiscard]] virtual u64 GetSize() const;
    protected:
        virtual void ReadImpl(const std::span<u8>& output, u64 offset) = 0;

        u64 size{};
        std::streamoff rd{}, wr{};
        u64 ioRecordUsage{};

        bool flushed{};

        DiskAccess privilege;
        std::filesystem::path diskPath;
    };
}