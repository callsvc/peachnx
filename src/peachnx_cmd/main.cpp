#include <memory>
#include <thread>

#include <boost/program_options.hpp>

#include <core/application.h>
#include <settings/configuration.h>
using namespace Peachnx;

i32 main(const i32 argc, char** argv) {
    namespace bpo = boost::program_options;

    bpo::options_description peachnx("Generic options");
    peachnx.add_options()
        ("game", bpo::value<std::string>(), "Specify the path of the game to be loaded");

    Settings::options = std::make_unique<Settings::Options>();
    const auto process{std::make_unique<Core::Application>()};
    auto emuSurface = [&] -> std::unique_ptr<Surface::SdlWindow> {
        const auto backend{Settings::options->surfaceBackend.GetValue()};
        if (backend == Settings::SurfaceBackend::OpenGl) {
            return std::make_unique<Surface::SdlImplOpenGl>();
        }
        return std::make_unique<Surface::SdlImplOpenGl>();
    }();

    bpo::variables_map vm;
    store(parse_command_line(argc, argv, peachnx), vm);
    vm.notify();

    std::string programPath;
    if (vm.contains("game")) {
        programPath = vm["game"].as<std::string>();
    }
    constexpr Service::AM::AppletParameters gameParams{
        .id = Service::AM::AppletKind::Application
    };
    process->MakeSwitchContext(std::move(emuSurface), programPath, gameParams);

    if (process->IsRunning())
        throw std::runtime_error("We shouldn't be running");

    std::this_thread::sleep_for(std::chrono::seconds(3));
}
