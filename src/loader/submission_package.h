#pragma once

#include <loader/loader.h>
namespace peachnx::loader {
    class SubmissionPackage final : public Loader {
    public:
        SubmissionPackage(const disk::VirtFilePtr& main, const LoaderExtra& loaderParams);
        static ApplicationType GetTypeFromFile(const disk::VirtFilePtr& probFile);
    };
}