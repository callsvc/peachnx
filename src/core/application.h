#pragma once

#include <atomic>
#include <core/assets_backing.h>
#include <core/games_list.h>

#include <surface/sdl_impl_gl.h>

#include <crypto/keysdb.h>
#include <kernel/kernel.h>

namespace Peachnx::Core {
    class Application {
    public:
        explicit Application(bool useTemp = false);
        ~Application();
        void MakeSwitchContext(std::unique_ptr<Surface::SdlWindow>&& window,
            const std::string& program, const Service::AM::AppletParameters& params);

        [[nodiscard]] bool IsRunning() const;
        std::vector<std::shared_ptr<Loader::Loader>> GetGameList();

    private:
        AssetsBacking assets;
        GamesList collection;

        Kernel::Kernel kernel;
        std::shared_ptr<Crypto::KeysDb> kdb;

        std::mutex processLock;
        std::atomic<bool> running;

        std::shared_ptr<Loader::Loader> appLoader;
        std::unique_ptr<Surface::SdlWindow> sdlScene;
    };
}