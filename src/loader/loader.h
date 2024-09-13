#pragma once

#include <disk/virtual_types.h>
namespace peachnx::loader {
    struct LoaderExtra {
        u64 programId;
        u64 programIndex;
    };
    enum class ApplicationType {
        Unrecognized,
        NSP // Nintendo Submission Package, normally downloaded from E-Shop titles, similar to .APK on Android or .APP on iOS
    };
    std::string GetApplicationStringType(ApplicationType type);

    ApplicationType IdentifyAppType(disk::VirtFilePtr& app);

    class Loader {
    public:
        Loader() = default;
    protected:
        virtual ~Loader() = default;
        static ApplicationType GetTypeFromFile(disk::VirtFilePtr& probFile);
    };

    std::shared_ptr<Loader> GetLoader(disk::VirtFilePtr& mainFile, LoaderExtra& extra);
}