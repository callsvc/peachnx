#include <loader/application.h>
#include <loader/submission_package.h>

namespace peachnx::loader {
    template <typename T> requires (std::derived_from<T, Application>)
    std::optional<ApplicationType> readAppType(disk::VirtFilePtr& app) {
        auto type{T::GetTypeFromFile(app)};
        if (type != ApplicationType::Unrecognized)
            return type;
        return {};
    }

    ApplicationType IdentifyAppType(disk::VirtFilePtr& app) {
        if (const auto fileType = readAppType<SubmissionPackage>(app))
            return *fileType;
        return ApplicationType::Unrecognized;
    }
}
