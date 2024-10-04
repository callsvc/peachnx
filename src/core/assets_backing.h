#pragma once

#include <sys_fs/realfs.h>
namespace Peachnx::Core {
    // This class will manage all the contents present in the application's root directory
    class AssetsBacking {
    public:
        AssetsBacking() = default;
        explicit AssetsBacking(const std::filesystem::path& root);

        std::filesystem::path games;
        std::filesystem::path keys;

        std::shared_ptr<SysFs::RealFs> backing;

        [[nodiscard]] SysFs::VirtFilePtr GetMainFileFromPath(const std::string& gamePath) const;
        [[nodiscard]] auto GetTitleKeys() const {
            return keys / "title.keys";
        }
        [[nodiscard]] auto GetProdKey() const {
            return keys / "prod.keys";
        }
    private:
        static void VerifyDirectory(const std::filesystem::path& cave);
    };
}

