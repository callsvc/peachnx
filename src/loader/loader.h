#pragma once

#include <crypto/keysdb.h>
#include <sys_fs/virtual_types.h>

#include <kernel/kprocess.h>
namespace Peachnx::Loader {
    enum class ApplicationType {
        Unrecognized,
        NSP, // Nintendo Submission Package, normally downloaded from E-Shop titles, similar to .APK on Android or .APP on iOS
        NCA
    };
    std::string GetApplicationStringType(ApplicationType type);

    ApplicationType IdentifyAppType(const SysFs::VirtFilePtr& app);

    class Loader {
    public:
        Loader() = default;

        static ApplicationType GetTypeFromFile(const SysFs::VirtFilePtr& probFile);

        virtual void LoadProcess(const std::shared_ptr<Kernel::KProcess>& proc) = 0;
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

    std::shared_ptr<Loader> GetLoader(std::shared_ptr<Crypto::KeysDb>& kdb, SysFs::VirtFilePtr& mainFile, u64 programId, u64 programIndex);
}