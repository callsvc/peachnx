#include <loader/content_archive.h>

namespace peachnx::loader {
    ContentArchive::ContentArchive(std::shared_ptr<crypto::KeysDb>& kdb, disk::VirtFilePtr& main) :
        nca(std::make_unique<NCA>(kdb, main)), file(main) {}

    ApplicationType ContentArchive::GetTypeFromFile(const disk::VirtFilePtr& probFile) {
        if ([[maybe_unused]] const auto magic = probFile->Read<u32>())
            return ApplicationType::NCA;
        return ApplicationType::Unrecognized;
    }
    std::vector<u8> ContentArchive::GetLogo() {
        if (const auto logoDir = nca->logo) {
            if (const auto super = logoDir->OpenFile("StartupMovie.gif"))
                return super->GetBytes(super->GetSize());
        }
        return {};
    }
    std::string ContentArchive::GetGameTitle() {
        return file->GetDiskPath();
    }
    void ContentArchive::LoadProcess(const std::shared_ptr<kernel::KProcess>& proc) {

    }
}
