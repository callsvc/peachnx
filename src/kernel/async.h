#pragma once
#include <functional>
#include <memory>

#include <container.h>
#include <boost/context/detail/fcontext.hpp>
namespace peachnx::kernel {
    constexpr auto defaultStackSize{512 * 1024};

    struct AsyncContext {
        explicit AsyncContext(const u64 stackSize) :
            stack(stackSize),
            stackRewind(stackSize) {

        }
        PageVector<u8> stack;
        PageVector<u8> stackRewind;

        std::function<void()> entryPoint;
    };
    namespace gt = boost::context::detail;

    class Async {
    public:
        Async() = default;
        explicit Async(std::function<void()>&& function);

        static void TrappedEntryPoint(gt::transfer_t context);
        std::unique_ptr<AsyncContext> container;
    };
}
