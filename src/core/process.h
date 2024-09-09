#pragma once

#include <atomic>
#include <core/assets_backing.h>
#include <surface/sdl_impl_gl.h>

#include <service/am/manager.h>
namespace peachnx::core {
    class Process {
    public:
        explicit Process(bool useTemp = false);
        ~Process();
        [[nodiscard]] bool IsRunning() const;
        void MakeSwitchContext(std::unique_ptr<surface::SdlWindow>&& window,
            const std::string& program, const service::am::AppletParameters& params);
    private:
        AssetsBacking assets;

        std::mutex processLock;
        std::atomic<bool> running;

        std::unique_ptr<surface::SdlWindow> emuWindow;
    };
}