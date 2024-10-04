#pragma once

#include <kernel/kernel.h>
namespace Peachnx::Kernel {
    class KProcess : public KBase {
    public:
        explicit KProcess(Kernel& self);
    };
}