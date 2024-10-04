#include <filesystem>
#include <SDL2/SDL.h>

#include <settings/configuration.h>
namespace Peachnx::Settings {

    namespace SystemDefault {
        auto GetInstallDirectory() {
            const std::string path{std::filesystem::current_path()};
            return path;
        }
        i32 GetWindowPos() {
            return SDL_WINDOWPOS_UNDEFINED;
        }
    }

    Options::Options() {
        installedDirectory = Option{"installed-directory", SystemDefault::GetInstallDirectory()};

        lastX = Option{"last-x", SystemDefault::GetWindowPos()};
        lastY = Option{"last-y", SystemDefault::GetWindowPos()};
        width = Option{"width", 640};
        height = Option{"height", 640};

        surfaceBackend = Option{"surface-backend", SurfaceBackend::OpenGl};
    }
}
