#pragma once

#include <cstring>
namespace peachnx::crypto {
    // https://github.com/Thealexbarney/LibHac/blob/master/KEYS.md
    using Key128 = std::array<u8, 16>;
    using Key256 = std::array<u8, 32>;

    struct Key128Hash {
        auto operator ()(const Key128& key) const {
            std::array<u64, 2> pairs;
            std::memcpy(&pairs[0], &key[0], sizeof(Key128));

            return std::hash<u64>()(pairs[0]) ^ std::hash<u64>()(pairs[1]);
        }
    };
}