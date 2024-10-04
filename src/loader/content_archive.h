#pragma once

#include <loader/loader.h>
#include <loader/nca.h>
namespace Peachnx::Loader {

    class ContentArchive final : public Loader {
    public:
        ContentArchive(std::shared_ptr<Crypto::KeysDb>& kdb, SysFs::VirtFilePtr& main);

        static ApplicationType GetTypeFromFile(const SysFs::VirtFilePtr& probFile);
        std::vector<u8> GetLogo() override;
        std::string GetGameTitle() override;

        void LoadProcess(const std::shared_ptr<Kernel::KProcess>& proc) override;

    private:
        std::unique_ptr<NCA> nca;
        SysFs::VirtFilePtr file;
    };
}