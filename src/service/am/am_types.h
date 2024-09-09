#pragma once

#include <types.h>
namespace peachnx::service::am {
    using AppletId = u64;
    enum class AppletKind : AppletId {
        None,
        Application
    };
    using ApplicationId = AppletId;
}