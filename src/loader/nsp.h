#pragma once

#include <crypto/keysdb.h>
#include <sys_fs/partition_filesystem.h>

#include <loader/nca.h>
#include <loader/content_meta.h>
namespace Peachnx::Loader {
    struct MetaNca {
        ContentType type;
    };
    class NSP final : public ComposedLoader {
    public:
        explicit NSP(const std::shared_ptr<Crypto::KeysDb>& kdb, const SysFs::VirtFilePtr& nsp, u64 titleId, u64 programIndex);
        explicit NSP(const SysFs::VirtFilePtr& nsp);

        void ReadContent(const boost::unordered_map<std::string, SysFs::VirtFilePtr>& files);
        bool CheckIntegrity() const override;
        std::vector<u64> GetProgramIds();

        std::unordered_map<u64, std::unique_ptr<NCA>> contents;
        std::vector<ContentMeta> cnmts;

        // Stores all NCAs based on their title ID
        std::unique_ptr<SysFs::PartitionFilesystem> pfs;
    private:
        std::shared_ptr<Crypto::KeysDb> keys;

        u64 program;
        u64 index;
    };
}