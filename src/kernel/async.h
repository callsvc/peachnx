#pragma once
#include <functional>

#include <common/common_types.h>
namespace peachnx::kernel {
    constexpr auto defaultStackSize{512 * 1024};
    class Async {
    public:
        Async() :
            stack(defaultStackSize),
            stackRewind(defaultStackSize) {}

        common::HugeVector<u8> stack;
        common::HugeVector<u8> stackRewind;
    private:
        std::function<void()> entryPoint;
    };
}
