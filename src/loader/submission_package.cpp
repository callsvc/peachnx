#include <ranges>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <loader/submission_package.h>
namespace Peachnx::Loader {
    SubmissionPackage::SubmissionPackage(std::shared_ptr<Crypto::KeysDb>& kdb, SysFs::VirtFilePtr& main, u64 programId, u64 programIndex) :
        nsp(std::make_unique<NSP>(kdb, main, programId, programIndex)), file(main) {
    }
    ApplicationType SubmissionPackage::GetTypeFromFile(const SysFs::VirtFilePtr& probFile) {
        const NSP package(probFile);
        if (u32 magic{}; probFile->GetSize() > sizeof(magic)) {
            magic = probFile->Read<u32>();
            if (magic == MakeMagic<u32>("PFS0") || magic == MakeMagic<u32>("HFS0"))
                if (package.CheckIntegrity())
                    return ApplicationType::NSP;
        }

        return ApplicationType::Unrecognized;
    }
    std::vector<u8> SubmissionPackage::GetLogo() {
        assert(nsp->CheckIntegrity());

        auto& nspContent{nsp->contents};
        for (const auto& [index, nca]: nspContent) {
            auto& dirs{nca->GetDirectories()};
            for (const auto& entry: dirs) {
                if (const auto logoFile{entry->OpenFile("StartupMovie.gif")})
                    return logoFile->GetBytes(logoFile->GetSize());
            }
        }
        return {};
    }
    std::string SubmissionPackage::GetGameTitle() {
        const std::string title{file->GetDiskPath().filename()};
        std::vector<std::string> titleName;

        split(titleName, title, boost::is_any_of(" ["));
        if (titleName.size() > 2) {
            titleName.resize(2);
            return std::format("{} {}", titleName.front(), titleName.back());
        }

        return titleName.front();
    }
    void SubmissionPackage::LoadProcess(const std::shared_ptr<Kernel::KProcess>& proc) {
        auto programIds{nsp->GetProgramIds()};

        // Let's load the first program in the list
        const auto targets{std::ranges::find_if(programIds, [&](const u64 program) { return !(program & 0x800); })};
        if (targets == programIds.end())
            return;
        if (nsp->contents.contains(*targets)) {
            [[maybe_unused]] auto& programNca{nsp->contents[*targets]};
        }
    }
}
