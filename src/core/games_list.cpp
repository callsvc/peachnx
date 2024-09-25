#include <core/games_list.h>
#include <functional>

namespace peachnx::core {
    GamesList::GamesList(const std::shared_ptr<crypto::KeysDb>& kdb, const disk::Path& dir) : keys(kdb) {
        gamesList.emplace_back(dir);
        using DirEntry = std::filesystem::directory_entry;

        std::function<void(const DirEntry&)> searchForGames = [&](const auto& gameDir) {
            std::filesystem::directory_iterator walker(gameDir);
            if (walker == std::filesystem::directory_iterator())
                return;

            for (const auto& entry: walker) {
                if (entry.is_directory()) {
                    searchForGames(entry);
                }
                service::am::AppletParameters defaultParams{};
                AddGame(std::make_shared<disk::RegularFile>(entry.path()), defaultParams);
            }
        };

        const DirEntry rootDir{dir};
        searchForGames(rootDir);
    }
    void GamesList::AddGame(const disk::VirtFilePtr& game, const service::am::AppletParameters& params) {
        loader::LoaderExtra options {
            params.programId, params.programIndex
        };

        games.emplace_back(game);
        loaders.emplace_back(GetLoader(keys, games.back(), options));
    }
}