#pragma once

#include <atomic>
#include <core/assets_backing.h>
#include <core/games_list.h>

#include <surface/sdl_impl_gl.h>

#include <crypto/keysdb.h>
#include <kernel/kernel.h>

namespace peachnx::core {
    class Application {
    public:
        explicit Application(bool useTemp = false);
        ~Application();
        void MakeSwitchContext(std::unique_ptr<surface::SdlWindow>&& window,
            const std::string& program, const service::am::AppletParameters& params);

        [[nodiscard]] bool IsRunning() const;
        [[nodiscard]] auto GetCollection() {
            return gamesList.loaders;
        }
    private:
        AssetsBacking assets;
        GamesList gamesList;

        kernel::Kernel kernel;
        std::shared_ptr<crypto::KeysDb> kdb;

        std::mutex processLock;
        std::atomic<bool> running;

        std::shared_ptr<loader::Loader> appLoader;
        std::unique_ptr<surface::SdlWindow> emuWindow;
    };
}