#include <types.h>
#include <surface/sdl_impl_gl.h>

namespace peachnx::surface {
    SdlWindow::SdlWindow() {
        constexpr u32 sdlFlags{SDL_INIT_VIDEO};
        if (!SDL_WasInit(sdlFlags)) {
            SDL_InitSubSystem(sdlFlags);
        }
    }
    SdlWindow::~SdlWindow() {
        SDL_Quit();
    }
    void SdlWindow::Show() {
        SDL_ShowWindow(sdlWindow);
        isHide = !isHide;
    }

}
