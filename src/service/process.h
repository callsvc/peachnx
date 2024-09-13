#pragma once
#include <kernel/kernel.h>
#include <loader/submission_package.h>

namespace peachnx::service {
    class Process {
    public:
        Process(const kernel::Kernel& kernel, const std::shared_ptr<loader::Loader>& loader);
    };
}