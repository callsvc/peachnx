#pragma once

#include <format>
#include <boost/algorithm/hex.hpp>

#include <types.h>
namespace peachnx {
    template <typename T>
    std::string ByteArrayToString(const T& byteArray) {
        std::string result;

        result.reserve(byteArray.size() * 2);
        for (const auto byte : byteArray) {
            result += std::format("{:02x}", byte);
        }
        return result;
    }

    template <u64 Size>
    auto StringToByteArray(const std::string_view& desired) {
        namespace bad = boost::algorithm::detail;
        std::array<u8, Size> result{};
        if (desired.size() / 2 != Size)
            return result;
        u32 resIndex{};
        for (decltype(Size) index{}; index < desired.size(); index += 2) {
            result[resIndex] = bad::hex_char_to_int(desired[index]) << 4;
            result[resIndex] |= bad::hex_char_to_int(desired[index + 1]) & 0xf;
            resIndex++;
        }

        return result;
    }
}