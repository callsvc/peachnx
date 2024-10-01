#pragma once

#include <loader/loader.h>
#include <kernel/kernel.h>

namespace peachnx::service {
    class MakeProcess {
        public:
            MakeProcess(const std::shared_ptr<loader::Loader>& loadable, kernel::Kernel& kernel);
    };
}
