#pragma once

#include <sys_fs/regular_file.h>
#include <sys_fs/offset_file.h>
namespace Peachnx::SysFs {
    using VirtFilePtr = std::shared_ptr<VirtualFile>;
    using RegularFilePtr = std::shared_ptr<RegularFile>;
    using OffsetFilePtr = std::shared_ptr<OffsetFile>;

    static_assert(std::is_convertible_v<OffsetFilePtr, VirtFilePtr>, "Unconvertible object type");

    using RegularFileWeak = std::weak_ptr<RegularFile>;

    using Path = std::filesystem::path;
}