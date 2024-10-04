#include <kernel/kprocess.h>
#include <service/make_process.h>

namespace Peachnx::Service {
    MakeProcess::MakeProcess(const std::shared_ptr<Loader::Loader>& loadable, Kernel::Kernel& kernel) {
        const auto process{std::make_shared<Kernel::KProcess>(kernel)};
        loadable->LoadProcess(process);
    }
}
