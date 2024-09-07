#pragma once

#include <disk/realfs.h>
namespace peachnx::core {
    // This class will manage all the contents present in the application's root directory
    class AssetsBacking {
    public:
        AssetsBacking() = default;
        explicit AssetsBacking(const std::filesystem::path& root);

        std::filesystem::path games;
        std::filesystem::path keys;

        std::shared_ptr<disk::RealFs> backing;
    private:
        static void VerifyDirectory(const std::filesystem::path& cave);
    };
}

