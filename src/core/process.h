#pragma once

#include <atomic>
#include <core/assets_backing.h>
#include <surface/sdl_impl_gl.h>
namespace peachnx::core {
    class Process {
    public:
        explicit Process(bool useTemp = false);
        bool IsRunning() const;
        void MakeSwitchContext(std::unique_ptr<surface::SdlWindow>&& window);
    private:
        AssetsBacking assets;

        std::mutex processLock;
        std::atomic<bool> running;

        std::unique_ptr<surface::SdlWindow> emuWindow;
    };
}