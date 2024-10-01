#pragma once

#include <loader/loader.h>
#include <loader/nca.h>
namespace peachnx::loader {

    class ContentArchive final : public Loader {
    public:
        ContentArchive(std::shared_ptr<crypto::KeysDb>& kdb, disk::VirtFilePtr& main);

        static ApplicationType GetTypeFromFile(const disk::VirtFilePtr& probFile);
        std::vector<u8> GetLogo() override;
        std::string GetGameTitle() override;

        void LoadProcess(const std::shared_ptr<kernel::KProcess>& proc) override;

    private:
        std::unique_ptr<NCA> nca;
        disk::VirtFilePtr file;
    };
}