#pragma once

namespace Peachnx::Kernel {
    class Kernel {
    public:
        Kernel() = default;
    };

    enum class KType {
        Undefined,
        KProcess
    };
    class KBase {
    public:
        KBase(Kernel& origin, const KType self) : kSuper(origin), type(self) {}

        Kernel& kSuper;
        const KType type;
    };
}