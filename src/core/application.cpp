#include <print>
#include <utility>
#include <unistd.h>

#include <core/application.h>
#include <settings/configuration.h>

#include <os/make_process.h>
namespace peachnx::core {
    Application::Application(const bool useTemp) {
        std::filesystem::path workDir;
        if (useTemp) {
            workDir = std::filesystem::temp_directory_path();
        } else {
            workDir = settings::options->installedDirectory.GetValue();
        }

        std::print("Process started with PID {} on CPU {}\n", getpid(), sched_getcpu());
        assets = AssetsBacking(workDir);

        kdb = std::make_shared<crypto::KeysDb>();
        kdb->Initialize(assets);

        collection = GamesList(kdb, assets.games);
    }
    Application::~Application() {
        std::print("Process stopped\n");
    }
    bool Application::IsRunning() const {
        return running.load(std::memory_order::relaxed);
    }

    std::vector<std::shared_ptr<loader::Loader>> Application::GetGameList() {
        std::vector<std::shared_ptr<loader::Loader>> games;
        for (const auto& [_, loader] : collection.cached) {
            games.emplace_back(loader);
        }

        return games;
    }

    void Application::MakeSwitchContext(std::unique_ptr<surface::SdlWindow>&& window,
        const std::string& program, const service::am::AppletParameters& params) {

        std::lock_guard lock{processLock};
        if (IsRunning())
            return;

        const auto mainFile{assets.GetMainFileFromPath(program)};
        collection.AddGame(mainFile, params);
        const auto games{GetGameList()};
        os::MakeProcess(games.back(), kernel);

        sdlScene = std::move(window);
        sdlScene->Show();
    }
}
