#pragma once
#include <format>
#include <cstdint>

namespace peachnx {
    constexpr auto majorVersion{0};
    constexpr auto minorVersion{1};
    constexpr auto patchVersion{0};

    inline auto titleWindow = [] {
        return std::format("Peachnx {}.{}.{}", majorVersion, minorVersion, patchVersion);
    }();

    using u8 = std::uint8_t;
    using u32 = std::uint32_t;
    using i32 = std::int32_t;
    using u64 = std::uint64_t;
}