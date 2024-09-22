#pragma once

#include <loader/loader.h>
#include <loader/nsp.h>

#include <crypto/keysdb.h>
namespace peachnx::loader {
    class SubmissionPackage final : public Loader {
    public:
        SubmissionPackage(std::shared_ptr<crypto::KeysDb>& kdb, disk::VirtFilePtr& main, const LoaderExtra& pkgParams);
        static ApplicationType GetTypeFromFile(const disk::VirtFilePtr& probFile);
    private:
        std::unique_ptr<NSP> nsp;
    };
}