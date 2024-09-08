#pragma once

#include <surface/sdl_window.h>
namespace peachnx::surface {
    class SdlImplOpenGl final : public SdlWindow {
    public:
        SdlImplOpenGl();
        ~SdlImplOpenGl() override;

    private:
        bool CheckForRequiredExtensions() override;

        SDL_GLContext* context{nullptr};
    };
}