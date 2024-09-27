#include <kernel/kprocess.h>
#include <os/make_process.h>

namespace peachnx::os {
    MakeProcess::MakeProcess([[maybe_unused]] std::shared_ptr<loader::Loader> loadable, kernel::Kernel& kernel) {

        auto process{std::make_unique<kernel::KProcess>(kernel)};
    }
}
