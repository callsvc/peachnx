#include <core/games_list.h>
#include <functional>

namespace Peachnx::Core {
    GamesList::GamesList(const std::shared_ptr<Crypto::KeysDb>& kdb, const SysFs::Path& dir) : keys(kdb) {
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
                Service::AM::AppletParameters defaultParams{};
                AddGame(std::make_shared<SysFs::RegularFile>(entry.path()), defaultParams);
            }
        };

        const DirEntry rootDir{dir};
        searchForGames(rootDir);
    }
    void GamesList::AddGame(const SysFs::VirtFilePtr& game, const Service::AM::AppletParameters& params) {
        games.emplace_back(game);
        cached.emplace_back(params, Loader::GetLoader(keys, games.back(), params.programId, params.programIndex));
    }
}