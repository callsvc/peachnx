#pragma once

#include <disk/virtual_types.h>
#include <loader/loader.h>
#include <service/am/applet_manager.h>
namespace peachnx::core {
    struct LoadableGame {
        service::am::AppletParameters params;
        std::shared_ptr<loader::Loader> contained;
    };
    class GamesList {
        public:
        GamesList() = default;
        explicit GamesList(const std::shared_ptr<crypto::KeysDb>& kdb, const disk::Path& dir);
        void AddGame(const disk::VirtFilePtr& game, const service::am::AppletParameters& params);

        std::vector<LoadableGame> cached;
    private:
        std::shared_ptr<crypto::KeysDb> keys;

        std::vector<disk::Path> dirsPack;
        std::vector<disk::VirtFilePtr> games;
    };
}