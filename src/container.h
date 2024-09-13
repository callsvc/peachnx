#pragma once

#include <vector>
#include <virtual_allocator.h>

namespace peachnx {
    template <typename T>
    struct MappableAllocator {
        MappableAllocator() = default;
        using value_type = T;
        static T* allocate(const u64 bytes) {
            const auto pointer{AllocateVirtualMemory(bytes)};
            if (!pointer) {
                throw std::bad_alloc();
            }
            return static_cast<T*>(pointer.value());
        }
        static void deallocate(T* pointer, const u64 bytes) {
            DeallocateVirtualMemory(pointer, bytes);
        }
    };

    template <typename T>
    using HugeVector = std::vector<T, MappableAllocator<T>>;
}