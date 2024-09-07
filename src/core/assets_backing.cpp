#include <print>

#include <core/assets_backing.h>
namespace peachnx::core {

    AssetsBacking::AssetsBacking(const std::filesystem::path& root) :
        backing(std::make_shared<disk::RealFs>(root)) {

        games = root / "games";
        keys = root / "keys";

        VerifyDirectory(games);
        VerifyDirectory(keys);
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
};