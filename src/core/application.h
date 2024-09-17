#pragma once

#include <atomic>
#include <core/assets_backing.h>
#include <surface/sdl_impl_gl.h>

#include <loader/loader.h>
#include <service/am/applet_manager.h>
#include <kernel/kernel.h>
#include <crypto/keysdb.h>
namespace peachnx::core {
    class Application {
    public:
        explicit Application(bool useTemp = false);
        ~Application();
        [[nodiscard]] bool IsRunning() const;
        void MakeSwitchContext(std::unique_ptr<surface::SdlWindow>&& window,
            const std::string& program, const service::am::AppletParameters& params);
    private:
        void LoadApplication(disk::VirtFilePtr& mainFile, const service::am::AppletParameters& params);
        AssetsBacking assets;
        kernel::Kernel kernel;
        crypto::KeysDb keys;

        std::mutex processLock;
        std::atomic<bool> running;

        std::shared_ptr<loader::Loader> appLoader;
        std::unique_ptr<surface::SdlWindow> emuWindow;
    };
}