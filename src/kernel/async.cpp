#include <kernel/async.h>

namespace peachnx::kernel {
    void Async::TrappedEntryPoint([[maybe_unused]] boost::context::detail::transfer_t context) {
    }

    Async::Async(std::function<void()>&& function) :
        container(std::make_unique<AsyncContext>(defaultStackSize)) {

        container->entryPoint = std::move(function);
        const auto stackBegin{&*std::end(container->stack)};
        make_fcontext(stackBegin, container->stack.size(), TrappedEntryPoint);
    }
}
