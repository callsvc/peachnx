#pragma once
#include <filesystem>
#include <vector>

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
        explicit VirtualFile(const std::filesystem::path& filename = {},
            DiskAccess access = DiskAccess::Read, bool tangible = false, u64 virtOff = 0, u64 virtSize = 0);
        virtual ~VirtualFile() = default;

        template <typename T> requires (std::is_trivially_copyable_v<T>)
        T Read(const u64 offset = 0) {
            T copyable;
            ReadImpl({reinterpret_cast<u8*>(&copyable), sizeof(copyable)}, offset);
            return copyable;
        }
        u64 Read(const std::span<u8> bytes, const u64 offset) {
            return ReadImpl(bytes, offset);
        }
        std::vector<u8> GetBytes(u64 size, u64 offset = {});

        [[nodiscard]] auto GetFilePrivilege() const {
            return privilege;
        }
        [[nodiscard]] auto GetDiskPath() const {
            return diskPath;
        }

        [[nodiscard]] virtual u64 GetSize() const {
            return size;
        }
    protected:
        virtual u64 ReadImpl(const std::span<u8>& output, u64 offset) = 0;
        virtual u64 GetBytesImpl(std::vector<u8>& output, u64 offset);

        u64 size{};
        u64 readOffset{}, writeOffset{};
        u64 ioRecordUsage{};

        bool flushed{};

        DiskAccess privilege;
        std::filesystem::path diskPath;
    };
}