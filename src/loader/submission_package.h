#pragma once

#include <loader/application.h>
namespace peachnx::loader {
    class SubmissionPackage final : public Application {
    public:
        static ApplicationType GetTypeFromFile(const disk::VirtFilePtr& probFile);
    };
}