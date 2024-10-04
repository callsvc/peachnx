#pragma once
#include <memory>

#include <settings/options.h>
namespace Peachnx::Settings {

    struct Options {
        Options();
        Option<std::string> installedDirectory;
        Option<i32> lastX;
        Option<i32> lastY;
        Option<i32> width;
        Option<i32> height;

        Option<SurfaceBackend> surfaceBackend;
    };

    inline std::unique_ptr<Options> options;
}