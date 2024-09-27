#pragma once

#include <crypto/keysdb.h>
#include <disk/virtual_types.h>

namespace peachnx::loader {
    enum class ApplicationType {
        Unrecognized,
        NSP // Nintendo Submission Package, normally downloaded from E-Shop titles, similar to .APK on Android or .APP on iOS
    };
    std::string GetApplicationStringType(ApplicationType type);

    ApplicationType IdentifyAppType(disk::VirtFilePtr& app);

    class Loader {
    public:
        Loader() = default;

        static ApplicationType GetTypeFromFile(disk::VirtFilePtr& probFile);
        virtual std::vector<u8> GetLogo() = 0;
        virtual std::string GetGameTitle() = 0;
    protected:
        virtual ~Loader() = default;
    };

    std::shared_ptr<Loader> GetLoader(std::shared_ptr<crypto::KeysDb>& kdb, disk::VirtFilePtr& mainFile, u64 programId, u64 programIndex);
}