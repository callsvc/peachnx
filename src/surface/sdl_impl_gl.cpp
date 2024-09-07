#include <utility>
#include <print>

#include <SDL2/SDL_opengl.h>

#include <surface/sdl_impl_gl.h>
#include <settings/configuration.h>
namespace peachnx::surface {
     SdlImplOpenGl::SdlImplOpenGl() {
         if (!SDL_WasInit(SDL_INIT_VIDEO))
             return;
         const auto& options = settings::options;
         constexpr auto windowFlags{SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE};
         sdlWindow = SDL_CreateWindow(&titleWindow[0], options->lastX.GetValue(), options->lastY.GetValue(), options->width.GetValue(), options->height.GetValue(), windowFlags);
         if (!sdlWindow) {
             throw std::runtime_error("Failed to create an SDL window");
         }

         context = static_cast<SDL_GLContext*>(SDL_GL_CreateContext(sdlWindow));
         const auto driverName{SDL_GetCurrentVideoDriver()};
         std::print("SDL2 found the {} video driver\n", driverName);

         initialized = true;
     }

    SdlImplOpenGl::~SdlImplOpenGl() {
        SDL_GL_DeleteContext(context);
        if (sdlWindow)
            SDL_DestroyWindow(std::exchange(sdlWindow, nullptr));
    }
}