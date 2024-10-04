#include <print>

#include <surface/sdl_impl_gl.h>
namespace Peachnx::Surface {
    constexpr auto sdlFlags{SDL_INIT_VIDEO};
    SdlWindow::SdlWindow() {
        if (!SDL_WasInit(sdlFlags)) {
            if (SDL_InitSubSystem(sdlFlags) != 0)
                std::print("{}\n", SDL_GetError());
        }
    }
    SdlWindow::~SdlWindow() {
        SDL_QuitSubSystem(sdlFlags);
        SDL_Quit();

        SDL_TLSCleanup();
    }
    void SdlWindow::Show() {
        SDL_ShowWindow(sdlWindow);
        isHide = !isHide;
    }

}
