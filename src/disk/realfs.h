#pragma once
#include <mutex>
#include <unordered_map>

#include <disk/virtual_types.h>
namespace peachnx::disk {
    class RealFs {
    public:
        RealFs() = default;
        explicit RealFs(const Path& rootDirectory);

        RegularFilePtr OpenRegular(const Path& filename, DiskAccess access);
    private:
        std::unordered_map<Path, RegularFileWeak> cached;
        std::mutex lockFsList;
    };
}