#pragma once

#include <service/am/am_types.h>
namespace Peachnx::Service::AM {
    struct AppletParameters {
        ApplicationId programId;
        AppletKind id;

        u64 programIndex;
    };
}