#include <print>

#include <loader/loader.h>
#include <loader/submission_package.h>
#include <loader/content_archive.h>
namespace Peachnx::Loader {
    template<typename T> requires(std::derived_from<T, Loader>)
    std::optional<ApplicationType> ReadAppType(SysFs::VirtFilePtr& app) {
        auto type{T::GetTypeFromFile(app)};
        if (type != ApplicationType::Unrecognized)
            return type;
        return {};
    }

    std::string GetApplicationStringType(ApplicationType type) {
        constexpr std::array appTypes{"Submission Package", "Content Archive"};

        const auto typeIndex{static_cast<u32>(type)};
        if (type == ApplicationType::Unrecognized || typeIndex > appTypes.size())
            return "Unknown";
        return appTypes[typeIndex - 1];
    }

    ApplicationType IdentifyAppType(SysFs::VirtFilePtr& app) {
        if (const auto fileType = ReadAppType<SubmissionPackage>(app))
            return *fileType;
        if (const auto fileType = ReadAppType<ContentArchive>(app))
            return *fileType;
        return ApplicationType::Unrecognized;
    }
    std::shared_ptr<Loader> GetLoader(std::shared_ptr<Crypto::KeysDb>& kdb, SysFs::VirtFilePtr& mainFile, u64 programId, u64 programIndex) {
        const auto type{IdentifyAppType(mainFile)};
        const auto typeByName = [&] {
            const auto& filename{mainFile->GetDiskPath()};
            const auto& extension{filename.extension()};

            if (extension == ".nsp")
                return ApplicationType::NSP;
            if (extension == ".nca")
                return ApplicationType::NCA;
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
                case ApplicationType::NCA:
                    return std::make_shared<ContentArchive>(kdb, mainFile);
                default:
                    return nullptr;
            }
        }();

        return loader;
    }
}
