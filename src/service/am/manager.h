#pragma once

#include <service/am/am_types.h>
namespace peachnx::service::am {
    struct AppletParameters {
        ApplicationId programId;
        AppletKind id;
    };
}