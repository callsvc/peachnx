#pragma once

#include <loader/loader.h>
#include <kernel/kernel.h>

namespace peachnx::os {
    class MakeProcess {
        public:
            MakeProcess(std::shared_ptr<loader::Loader> loadable, kernel::Kernel& kernel);
    };
}
