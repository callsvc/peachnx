#include <format>

#include <types.h>
namespace Peachnx {
    std::string GetTitleWindow() {
        return std::format("Peachnx {}.{}.{}", majorVersion, minorVersion, patchVersion);
    }
}