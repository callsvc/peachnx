#include <print>
#include <unistd.h>

#include <core/process.h>
#include <settings/configuration.h>
namespace peachnx::core {
    Process::Process(const bool useTemp) {
        std::filesystem::path workDir;
        if (useTemp) {
            workDir = std::filesystem::temp_directory_path();
        } else {
            workDir = settings::options->installedDirectory.GetValue();
        }

        std::print("Process started with PID {} on CPU {}\n", getpid(), sched_getcpu());
        assets = AssetsBacking(workDir);
    }
    Process::~Process() {
        std::print("Process stopped\n");
    }
    bool Process::IsRunning() const {
        return running.load(std::memory_order::relaxed);
    }

    void Process::MakeSwitchContext(std::unique_ptr<surface::SdlWindow>&& window,
            const std::string& program, const service::am::AppletParameters& params) {
        (void)program;
        (void)params;

        std::lock_guard lock(processLock);

        if (IsRunning()) {
            return;
        }
        emuWindow = std::move(window);
        emuWindow->Show();
    }
}