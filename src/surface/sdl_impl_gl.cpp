#include <print>
#include <utility>
#include <vector>

#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>

#include <settings/configuration.h>
#include <surface/sdl_impl_gl.h>
namespace peachnx::surface {
    SdlImplOpenGl::SdlImplOpenGl() {
        if (!SDL_WasInit(SDL_INIT_VIDEO))
            return;
        const auto& options = settings::options;
        constexpr auto windowFlags{SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE};

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

        const auto title{GetTitleWindow()};
        sdlWindow = SDL_CreateWindow(&title[0], options->lastX.GetValue(), options->lastY.GetValue(), options->width.GetValue(), options->height.GetValue(), windowFlags);
        if (!sdlWindow) {
            throw std::runtime_error("Failed to create an SDL window");
        }

        context = static_cast<SDL_GLContext*>(SDL_GL_CreateContext(sdlWindow));
        const auto driverName{SDL_GetCurrentVideoDriver()};
        std::print("SDL2 found the {} video driver\n", driverName);

        if (!CheckForRequiredExtensions()) {
            throw std::runtime_error("Some required extensions were not found");
        }

        initialized = true;
    }

    SdlImplOpenGl::~SdlImplOpenGl() {
        SDL_GL_DeleteContext(context);
        if (sdlWindow)
            SDL_DestroyWindow(std::exchange(sdlWindow, nullptr));
    }
    bool SdlImplOpenGl::CheckForRequiredExtensions() {
        i32 extCount;
        glGetIntegerv(GL_NUM_EXTENSIONS, &extCount);

        std::vector<const char*> extensions;
        extensions.reserve(extCount);
        for (u32 ext{}; ext < extCount; ext++) {
            const auto extValue{glGetStringi(GL_EXTENSIONS, ext)};
            extensions.emplace_back(reinterpret_cast<const char*>(extValue));
        }

        std::vector<std::string> requiredExt{
            "GL_EXT_texture_compression_s3tc",
            "GL_ARB_texture_compression_rgtc",
            "GL_EXT_texture_filter_anisotropic"
        };
        for (const auto& extName : extensions) {
            if (requiredExt.empty())
                break;
            if (std::erase(requiredExt, extName)) {}
        }

        for (const auto& unsupported : requiredExt) {
            static const auto render{reinterpret_cast<const char*>(glGetString(GL_RENDERER))};
            std::print("Your OpenGL driver, named {}, does not support the required extension {}", render, unsupported);
        }

        return requiredExt.empty();
    }
}
