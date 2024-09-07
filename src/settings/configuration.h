#pragma once
#include <memory>

#include <settings/options.h>
namespace peachnx::settings {

    template <typename T>
    using OptOption = std::optional<Option<T>>;
    struct Options {
        Options();
        OptOption<std::string> installedDirectory;
        OptOption<i32> lastX;
        OptOption<i32> lastY;
        OptOption<i32> width;
        OptOption<i32> height;

        OptOption<SurfaceBackend> surfaceBackend;
    };

    extern std::unique_ptr<Options> options;
}