#include <core/games_list.h>
#include <functional>

namespace peachnx::core {
    GamesList::GamesList(const std::shared_ptr<crypto::KeysDb>& kdb, const disk::Path& dir) : keys(kdb) {
        dirsPack.emplace_back(dir);
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
        games.emplace_back(game);
        cached.emplace_back(params, loader::GetLoader(keys, games.back(), params.programId, params.programIndex));
    }
}