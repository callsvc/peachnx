#pragma once
#include <kernel/kernel.h>
#include <loader/submission_package.h>

namespace peachnx::service {
    class Process {
    public:
        Process([[maybe_unused]] const kernel::Kernel& kernel, [[maybe_unused]] const std::shared_ptr<loader::Loader>& loader) {}
    };
}