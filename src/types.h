#pragma once
#include <string>
#include <cstdint>
#include <cstring>

namespace peachnx {
    template <typename T> requires (std::is_unsigned_v<T>)
    T MakeMagic(const std::string_view& magicAsStr) {
        if (magicAsStr.size() < sizeof(T))
            return {};
        T magic{};
        std::memcpy(&magic, &magicAsStr[0], sizeof(T));
        return magic;
    }

    using u8 = std::uint8_t;
    using u32 = std::uint32_t;
    using i32 = std::int32_t;
    using u64 = std::uint64_t;

    constexpr auto majorVersion{0};
    constexpr auto minorVersion{1};
    constexpr auto patchVersion{0};

    std::string GetTitleWindow();
}