#include <disk/realfs.h>
namespace peachnx::disk {
    RealFs::RealFs(const Path& rootDirectory) {
        if (!exists(rootDirectory) && rootDirectory.parent_path() != "/") {
            create_directory(rootDirectory);
        }
    }

    RegularFilePtr RealFs::OpenRegular(const Path& filename, const DiskAccess access) {
        auto reqFileAccessMode = [&](RegularFilePtr& filePtr) {
            if (filePtr->GetFilePrivilege() == access)
                return std::optional(filePtr);

            filePtr->RequestADifferentFileMode(access);
            return std::optional<RegularFilePtr>{};
        };
        RegularFilePtr reference;

        std::scoped_lock lock(lockFsList);
        if (const auto cachedFile = cached.find(filename); cachedFile != cached.end()) {
            if (auto liveFile = cachedFile->second.lock())
                if (const auto sanitized = reqFileAccessMode(liveFile))
                    reference = *sanitized;
        }
        if (reference)
            return reference;

        reference = std::make_shared<RegularFile>(filename, access);

        cached[filename] = reference;
        return reference;
    }
}
