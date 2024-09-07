#include <memory>

#include <core/process.h>
#include <settings/configuration.h>
using namespace peachnx;
i32 main() {
    settings::options = std::make_unique<settings::Options>();

    const auto process{std::make_unique<core::Process>()};

    auto emuSurface = [&] -> std::unique_ptr<surface::SdlWindow> {
        const auto backend{settings::options->surfaceBackend->Get()};

        if (backend == settings::SurfaceBackend::OpenGl) {
            return std::make_unique<surface::SdlImplOpenGl>();
        }
        return std::make_unique<surface::SdlImplOpenGl>();
    }();
    process->MakeSwitchContext(std::move(emuSurface));

    if (process->IsRunning())
        throw std::runtime_error("We shouldn't be running");
}
