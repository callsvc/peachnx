#pragma once

#include <types.h>
namespace Peachnx::Service::AM {
    using AppletId = u64;
    enum class AppletKind : AppletId {
        None,
        Application
    };
    using ApplicationId = AppletId;
}