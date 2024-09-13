#include <cstring>

#include <loader/submission_package.h>
namespace peachnx::loader {
    template <typename T> requires (std::is_unsigned_v<T>)
    T MakeMagic(const std::string_view& magicAsStr) {
        if (magicAsStr.size() < sizeof(T))
            return {};
        T magic{};
        std::memcpy(&magic, &magicAsStr[0], sizeof(T));
        return magic;
    }

    SubmissionPackage::SubmissionPackage([[maybe_unused]] const disk::VirtFilePtr& main, [[maybe_unused]] const LoaderExtra& loaderParams) {
        (void)main;
        (void)loaderParams;
    }
    ApplicationType SubmissionPackage::GetTypeFromFile(const disk::VirtFilePtr& probFile) {
        if (u32 magic{}; probFile->GetSize() > sizeof(magic)) {
            magic = probFile->Read<u32>();
            if (magic == MakeMagic<u32>("NSP0"))
                return ApplicationType::NSP;
        }

        return ApplicationType::Unrecognized;
    }
}
