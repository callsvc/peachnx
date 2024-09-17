
#include <loader/submission_package.h>
namespace peachnx::loader {
    SubmissionPackage::SubmissionPackage(crypto::KeysDb& keysMgr, disk::VirtFilePtr& main, const LoaderExtra& params) :
        nsp(std::make_unique<NSP>(keysMgr, main, params.programId, params.programIndex)) {
    }
    ApplicationType SubmissionPackage::GetTypeFromFile(const disk::VirtFilePtr& probFile) {
        const NSP package(probFile);
        if (u32 magic{}; probFile->GetSize() > sizeof(magic)) {
            magic = probFile->Read<u32>();
            if (magic == MakeMagic<u32>("PFS0") || magic == MakeMagic<u32>("HFS0"))
                if (package.IsValidNsp())
                    return ApplicationType::NSP;
        }

        return ApplicationType::Unrecognized;
    }
}
