#include <print>
#include <utility>
#include <unistd.h>

#include <core/application.h>
#include <settings/configuration.h>

#include <service/make_process.h>
namespace Peachnx::Core {
    Application::Application(const bool useTemp) {
        std::filesystem::path workDir;
        if (useTemp) {
            workDir = std::filesystem::temp_directory_path();
        } else {
            workDir = Settings::options->installedDirectory.GetValue();
        }

        std::print("Process started with PID {} on CPU {}\n", getpid(), sched_getcpu());
        assets = AssetsBacking(workDir);

        kdb = std::make_shared<Crypto::KeysDb>();
        kdb->Initialize(assets);

        collection = GamesList(kdb, assets.games);
    }
    Application::~Application() {
        std::print("Process stopped\n");
    }
    bool Application::IsRunning() const {
        return running.load(std::memory_order::relaxed);
    }

    std::vector<std::shared_ptr<Loader::Loader>> Application::GetGameList() {
        std::vector<std::shared_ptr<Loader::Loader>> games;
        for (const auto& [_, loader] : collection.cached) {
            games.emplace_back(loader);
        }

        return games;
    }

    void Application::MakeSwitchContext(std::unique_ptr<Surface::SdlWindow>&& window,
        const std::string& program, const Service::AM::AppletParameters& params) {

        std::lock_guard lock{processLock};
        if (IsRunning())
            return;

        if (!program.empty()) {
            const auto mainFile{assets.GetMainFileFromPath(program)};
            collection.AddGame(mainFile, params);
        }
        const auto games{GetGameList()};
        if (games.empty()) {
            throw std::runtime_error("No games found");
        }
        Service::MakeProcess(games.back(), kernel);

        sdlScene = std::move(window);
        sdlScene->Show();
    }
}
