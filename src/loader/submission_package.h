#pragma once

#include <loader/loader.h>
#include <loader/nsp.h>
namespace peachnx::loader {
    class SubmissionPackage final : public Loader {
    public:
        SubmissionPackage(disk::VirtFilePtr& main, const LoaderExtra& pkgParams);
        static ApplicationType GetTypeFromFile(const disk::VirtFilePtr& probFile);
    private:
        std::unique_ptr<NSP> nsp;
    };
}