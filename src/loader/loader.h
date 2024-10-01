#pragma once

#include <crypto/keysdb.h>
#include <disk/virtual_types.h>

namespace peachnx::loader {
    enum class ApplicationType {
        Unrecognized,
        NSP, // Nintendo Submission Package, normally downloaded from E-Shop titles, similar to .APK on Android or .APP on iOS
        NCA
    };
    std::string GetApplicationStringType(ApplicationType type);

    ApplicationType IdentifyAppType(const disk::VirtFilePtr& app);

    class Loader {
    public:
        Loader() = default;

        static ApplicationType GetTypeFromFile(const disk::VirtFilePtr& probFile);
        virtual std::vector<u8> GetLogo() {
            __builtin_trap();
        }
        virtual std::string GetGameTitle() {
            __builtin_trap();
        }
        virtual bool IsLoaded() const {
            __builtin_trap();
        }
    protected:
        virtual ~Loader() = default;
    };

    std::shared_ptr<Loader> GetLoader(std::shared_ptr<crypto::KeysDb>& kdb, disk::VirtFilePtr& mainFile, u64 programId, u64 programIndex);
}