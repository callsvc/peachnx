#pragma once

#include <surface/sdl_window.h>
namespace Peachnx::Surface {
    class SdlImplOpenGl final : public SdlWindow {
    public:
        SdlImplOpenGl();
        ~SdlImplOpenGl() override;

    private:
        bool CheckForRequiredExtensions() override;

        SDL_GLContext* context{nullptr};
    };
}