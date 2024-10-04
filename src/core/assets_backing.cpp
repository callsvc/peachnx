#include <print>

#include <core/assets_backing.h>
namespace Peachnx::Core {

    AssetsBacking::AssetsBacking(const std::filesystem::path& root) :
        backing(std::make_shared<SysFs::RealFs>(root)) {

        games = root / "games";
        keys = root / "keys";

        VerifyDirectory(games);
        VerifyDirectory(keys);
    }
    SysFs::VirtFilePtr AssetsBacking::GetMainFileFromPath(const std::string& gamePath) const {
        const SysFs::Path userPath{gamePath};
        if (!exists(userPath))
            throw std::runtime_error("Path does not exist");

        if (is_directory(userPath)) {
            return backing->OpenRegular(gamePath + "/main", SysFs::DiskAccess::Read);
        }
        return backing->OpenRegular(gamePath, SysFs::DiskAccess::Read);
    }

    void AssetsBacking::VerifyDirectory(const std::filesystem::path& cave) {
        if (!exists(cave.parent_path()))
            throw std::runtime_error("Parent directory does not exist or is not accessible");

        if (!exists(cave)) {
            if (!create_directory(cave))
                throw std::runtime_error("Failed to create the directory");
            std::print("The directory {} has been installed\n", cave.string());
        }
    }
}