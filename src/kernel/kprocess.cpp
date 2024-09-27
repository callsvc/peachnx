#include <kernel/kprocess.h>

namespace peachnx::kernel {
    KProcess::KProcess(Kernel& self) : KBase(self, KType::KProcess) {
    }
}
