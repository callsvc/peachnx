#pragma once
#include <optional>

#include <types.h>
namespace Peachnx {
    std::optional<void*> AllocateVirtualMemory(u64 pointerSize);
    void DeallocateVirtualMemory(void* pointer, u64 pointerSize);

}