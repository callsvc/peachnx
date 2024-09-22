#include <print>
#include <unistd.h>

#include <core/application.h>
#include <settings/configuration.h>

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
    }
    Application::~Application() {
        std::print("Process stopped\n");
    }
    bool Application::IsRunning() const {
        return running.load(std::memory_order::relaxed);
    }

    void Application::MakeSwitchContext(std::unique_ptr<surface::SdlWindow>&& window,
        const std::string& program, const service::am::AppletParameters& params) {

        std::lock_guard lock{processLock};
        auto mainFile{assets.GetMainFileFromPath(program)};
        LoadApplication(mainFile, params);

        if (IsRunning()) {
            return;
        }
        emuWindow = std::move(window);
        emuWindow->Show();
    }

    void Application::LoadApplication(disk::VirtFilePtr& mainFile, const service::am::AppletParameters& params) {
        loader::LoaderExtra parameters {
            params.programId, params.programIndex
        };
        appLoader = GetLoader(kdb, mainFile, parameters);
    }

}
