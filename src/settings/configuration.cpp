#include <filesystem>
#include <SDL2/SDL.h>

#include <settings/configuration.h>
namespace peachnx::settings {

    namespace system_default {
        auto GetInstallDirectory() {
            const std::string path{std::filesystem::current_path()};
            return path;
        }
        i32 GetWindowPos() {
            return SDL_WINDOWPOS_UNDEFINED;
        }
    }

    Options::Options() {
        installedDirectory = Option{"installed-directory", system_default::GetInstallDirectory()};

        lastX = Option{"last-x", 640};
        lastY = Option{"last-y", 640};
        width = Option{"width", system_default::GetWindowPos()};
        height = Option{"height", system_default::GetWindowPos()};

        surfaceBackend = Option{"surface-backend", SurfaceBackend::OpenGl};
    }
}
