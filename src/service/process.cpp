#include <service/process.h>

namespace peachnx::service {
    Process::Process(const kernel::Kernel& kernel, const std::shared_ptr<loader::Loader>& loader) {
        (void)kernel;
        (void)loader;
    }
}
