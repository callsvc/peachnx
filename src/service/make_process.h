#pragma once

#include <loader/loader.h>
#include <kernel/kernel.h>

namespace Peachnx::Service {
    class MakeProcess {
        public:
            MakeProcess(const std::shared_ptr<Loader::Loader>& loadable, Kernel::Kernel& kernel);
    };
}
