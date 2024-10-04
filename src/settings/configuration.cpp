#include <filesystem>
#include <SDL2/SDL.h>

#include <settings/configuration.h>
namespace Peachnx::Settings {

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

        lastX = Option{"last-x", system_default::GetWindowPos()};
        lastY = Option{"last-y", system_default::GetWindowPos()};
        width = Option{"width", 640};
        height = Option{"height", 640};

        surfaceBackend = Option{"surface-backend", SurfaceBackend::OpenGl};
    }
}
