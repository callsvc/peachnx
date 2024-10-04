#pragma once

#include <sys_fs/virtual_types.h>
#include <loader/loader.h>
#include <service/am/applet_manager.h>
namespace Peachnx::Core {
    struct LoadableGame {
        Service::AM::AppletParameters params;
        std::shared_ptr<Loader::Loader> contained;
    };
    class GamesList {
        public:
        GamesList() = default;
        explicit GamesList(const std::shared_ptr<Crypto::KeysDb>& kdb, const SysFs::Path& dir);
        void AddGame(const SysFs::VirtFilePtr& game, const Service::AM::AppletParameters& params);

        std::vector<LoadableGame> cached;
    private:
        std::shared_ptr<Crypto::KeysDb> keys;

        std::vector<SysFs::Path> dirsPack;
        std::vector<SysFs::VirtFilePtr> games;
    };
}