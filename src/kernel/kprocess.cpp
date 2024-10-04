#include <kernel/kprocess.h>

namespace Peachnx::Kernel {
    KProcess::KProcess(Kernel& self) : KBase(self, KType::KProcess) {
    }
}
