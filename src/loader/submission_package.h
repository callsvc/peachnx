#pragma once

#include <loader/loader.h>
#include <loader/nsp.h>

#include <crypto/keysdb.h>
namespace peachnx::loader {
    class SubmissionPackage final : public Loader {
    public:
        SubmissionPackage(std::shared_ptr<crypto::KeysDb>& kdb, disk::VirtFilePtr& main, const LoaderExtra& pkgParams);
        static ApplicationType GetTypeFromFile(const disk::VirtFilePtr& probFile);

        std::vector<u8> GetLogo() override;
        std::string GetGameTitle() override;

    private:
        std::unique_ptr<NSP> nsp;
        disk::VirtFilePtr file;
    };
}