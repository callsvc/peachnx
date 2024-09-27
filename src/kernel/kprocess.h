#pragma once

#include <kernel/kernel.h>
namespace peachnx::kernel {
    class KProcess : public KBase {
    public:
        explicit KProcess(Kernel& self);
    };
}