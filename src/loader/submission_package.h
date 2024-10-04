#pragma once

#include <loader/nsp.h>

#include <crypto/keysdb.h>
namespace Peachnx::Loader {
    class SubmissionPackage final : public Loader {
    public:
        SubmissionPackage(std::shared_ptr<Crypto::KeysDb>& kdb, SysFs::VirtFilePtr& main, u64 programId, u64 programIndex);
        static ApplicationType GetTypeFromFile(const SysFs::VirtFilePtr& probFile);

        std::vector<u8> GetLogo() override;
        std::string GetGameTitle() override;

        void LoadProcess(const std::shared_ptr<Kernel::KProcess>& proc) override;

    private:
        std::unique_ptr<NSP> nsp;
        SysFs::VirtFilePtr file;
    };
}