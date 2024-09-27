#include <vector>
#include <print>

#include <loader/loader.h>
#include <loader/submission_package.h>
namespace peachnx::loader {
    template<typename T> requires(std::derived_from<T, Loader>)
    std::optional<ApplicationType> readAppType(disk::VirtFilePtr& app) {
        auto type{T::GetTypeFromFile(app)};
        if (type != ApplicationType::Unrecognized)
            return type;
        return {};
    }

    std::string GetApplicationStringType(ApplicationType type) {
        static std::vector<std::string> appTypes{
            "NSP"
        };
        const auto typeIndex{static_cast<u32>(type)};
        if (type == ApplicationType::Unrecognized || typeIndex > appTypes.size())
            return "Unknown";
        return appTypes[typeIndex - 1];
    }

    ApplicationType IdentifyAppType(disk::VirtFilePtr& app) {
        if (const auto fileType = readAppType<SubmissionPackage>(app))
            return *fileType;
        return ApplicationType::Unrecognized;
    }
    std::shared_ptr<Loader> GetLoader(std::shared_ptr<crypto::KeysDb>& kdb, disk::VirtFilePtr& mainFile, u64 programId, u64 programIndex) {
        const auto type{IdentifyAppType(mainFile)};
        const auto typeByName = [&] {
            const auto& filename{mainFile->GetDiskPath()};
            const auto& extension{filename.extension()};

            if (extension == ".nsp")
                return ApplicationType::NSP;
            return ApplicationType::Unrecognized;
        }();

        if (type != typeByName) {
            throw std::runtime_error("The application type and its name differ");
        }

        const auto& appPath{mainFile->GetDiskPath()};
        std::print("Loading the {} application of type {}\n", appPath.string(), GetApplicationStringType(type));

        auto loader = [&] -> std::shared_ptr<Loader> {
            switch (type) {
                case ApplicationType::NSP:
                    return std::make_shared<SubmissionPackage>(kdb, mainFile, programId, programIndex);
                default:
                    return nullptr;
            }
        }();

        return loader;
    }
}
