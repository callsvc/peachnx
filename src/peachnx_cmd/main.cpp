#include <memory>
#include <thread>

#include <boost/program_options.hpp>

#include <core/application.h>
#include <settings/configuration.h>
using namespace peachnx;

i32 main(const i32 argc, char** argv) {
    namespace bpo = boost::program_options;

    bpo::options_description peachnx("Generic options");
    peachnx.add_options()
        ("game", bpo::value<std::string>(), "Specify the path of the game to be loaded");

    settings::options = std::make_unique<settings::Options>();
    const auto process{std::make_unique<core::Application>()};
    auto emuSurface = [&] -> std::unique_ptr<surface::SdlWindow> {
        const auto backend{settings::options->surfaceBackend.GetValue()};
        if (backend == settings::SurfaceBackend::OpenGl) {
            return std::make_unique<surface::SdlImplOpenGl>();
        }
        return std::make_unique<surface::SdlImplOpenGl>();
    }();

    bpo::variables_map vm;
    store(parse_command_line(argc, argv, peachnx), vm);
    vm.notify();

    std::string programPath;
    if (vm.contains("game")) {
        programPath = vm["game"].as<std::string>();
    }
    constexpr service::am::AppletParameters gameParams{
        .id = service::am::AppletKind::Application
    };
    process->MakeSwitchContext(std::move(emuSurface), programPath, gameParams);

    if (process->IsRunning())
        throw std::runtime_error("We shouldn't be running");

    std::this_thread::sleep_for(std::chrono::seconds(3));
}
