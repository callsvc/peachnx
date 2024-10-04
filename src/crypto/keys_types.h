#pragma once

#include <array>
#include <cstring>
#include <types.h>
namespace Peachnx::Crypto {
    // https://github.com/Thealexbarney/LibHac/blob/master/KEYS.md
    using Key128 = std::array<u8, 16>;
    using Key256 = std::array<u8, 32>;

    enum IndexedKey128Type : u8 {
        Application,
        Ocean,
        System,
        Titlekek // > From here, these values are not used directly by the specification
    };

    struct Key128Hash {
        auto operator ()(const Key128& key) const {
            std::array<u64, 2> pairs;
            std::memcpy(&pairs[0], &key[0], sizeof(Key128));

            return std::hash<u64>()(pairs[0]) ^ std::hash<u64>()(pairs[1]);
        }
    };

    bool KeyIsEmpty(const Key128& key);
}