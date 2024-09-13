#include <sys/mman.h>
#include <boost/align/align_up.hpp>

#include <virtual_allocator.h>
namespace peachnx {

    std::optional<void*> AllocateVirtualMemory(u64 pointerSize) {
        const auto pageSize{sysconf(_SC_PAGESIZE)};
        pointerSize = boost::alignment::align_up(pointerSize, pageSize);
#if defined(__linux__)
        auto memory{mmap(nullptr, pointerSize, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)};

        if (memory == MAP_FAILED) {
            memory = {};
        }
#endif
        return memory;
    }
    void DeallocateVirtualMemory(void* pointer, u64 pointerSize) {
        if (!boost::alignment::detail::is_alignment(pointerSize))
            pointerSize = boost::alignment::align_up(pointerSize, sysconf(_SC_PAGESIZE));
#if defined(__linux__)
        munmap(pointer, pointerSize);
#endif
    }
}
