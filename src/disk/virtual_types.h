#pragma once

#include <disk/regular_file.h>
#include <disk/offset_file.h>
namespace peachnx::disk {
    using VirtFilePtr = std::shared_ptr<VirtualFile>;
    using RegularFilePtr = std::shared_ptr<RegularFile>;
    using OffsetFilePtr = std::shared_ptr<OffsetFile>;

    using RegularFileWeak = std::weak_ptr<RegularFile>;

    using Path = std::filesystem::path;
}