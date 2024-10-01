#include <kernel/kprocess.h>
#include <service/make_process.h>

namespace peachnx::service {
    MakeProcess::MakeProcess(const std::shared_ptr<loader::Loader>& loadable, kernel::Kernel& kernel) {
        const auto process{std::make_shared<kernel::KProcess>(kernel)};
        loadable->LoadProcess(process);
    }
}
