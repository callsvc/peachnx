#pragma once

#include <types.h>
namespace Peachnx::Settings {
    enum class OptionType : u32 {
        None,
        String,
        Integer
    };

    enum class SurfaceBackend : u32 {
        Null,
        OpenGl,
    };

}
