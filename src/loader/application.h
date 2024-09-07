#pragma once

#include <disk/virtual_types.h>
namespace peachnx::loader {
    enum class ApplicationType {
        Unrecognized,
        NSP // Nintendo Submission Package, normally downloaded from E-Shop titles, similar to .APK on Android or .APP on iOS
    };

    ApplicationType IdentifyAppType(disk::VirtFilePtr& app);

    class Application {
    public:
        Application() = default;
    protected:
        virtual ~Application() = default;
        static ApplicationType GetTypeFromFile(disk::VirtFilePtr& probFile);
    };
}