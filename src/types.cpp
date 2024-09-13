#include <format>

#include <types.h>
namespace peachnx {
    std::string GetTitleWindow() {
        return std::format("Peachnx {}.{}.{}", majorVersion, minorVersion, patchVersion);
    };
}