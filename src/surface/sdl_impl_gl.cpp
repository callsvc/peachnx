#include <utility>

#include <surface/sdl_impl_gl.h>
#include <settings/configuration.h>
namespace peachnx::surface {
     SdlImplOpenGl::SdlImplOpenGl() : SdlWindow() {
         if (!SDL_WasInit(SDL_INIT_VIDEO))
             return;

         const auto lastXPos{settings::options->lastX->Get()};
         const auto lastYPos{settings::options->lastY->Get()};
         const auto width{settings::options->width->Get()};
         const auto height{settings::options->height->Get()};

         surface = SDL_CreateWindow("Peachnx", lastXPos, lastYPos, width, height, SDL_WINDOW_OPENGL);
     }

    SdlImplOpenGl::~SdlImplOpenGl() {
        if (surface)
            SDL_DestroyWindow(std::exchange(surface, nullptr));
    }
}