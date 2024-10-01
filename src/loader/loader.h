#pragma once

#include <crypto/keysdb.h>
#include <disk/virtual_types.h>

#include <kernel/kprocess.h>
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

        virtual void LoadProcess(const std::shared_ptr<kernel::KProcess>& proc) = 0;
        virtual std::vector<u8> GetLogo() = 0;
        virtual std::string GetGameTitle() = 0;
    protected:
        virtual ~Loader() = default;
    };

    class ComposedLoader {
    public:
        virtual ~ComposedLoader() = default;
        virtual bool CheckIntegrity() const = 0;
    };

    std::shared_ptr<Loader> GetLoader(std::shared_ptr<crypto::KeysDb>& kdb, disk::VirtFilePtr& mainFile, u64 programId, u64 programIndex);
}