#pragma once

#include <crypto/keysdb.h>
#include <disk/partition_filesystem.h>

#include <loader/nca.h>
#include <disk/content_meta.h>
namespace peachnx::loader {
    class NSP final : public ComposedLoader {
    public:
        explicit NSP(const std::shared_ptr<crypto::KeysDb>& kdb, const disk::VirtFilePtr& nsp, u64 titleId, u64 programIndex);
        explicit NSP(const disk::VirtFilePtr& nsp);

        void ReadContent(const boost::unordered_map<std::string, disk::VirtFilePtr>& files);
        bool CheckIntegrity() const override;
        std::vector<u64> GetProgramIds();

        std::vector<std::unique_ptr<NCA>> contents;
        std::vector<disk::ContentMeta> cnmts;

        // Stores all NCAs based on their title ID
        std::unordered_map<u64, u64> indexedNca;

        std::unique_ptr<disk::PartitionFilesystem> pfs;
    private:
        std::shared_ptr<crypto::KeysDb> keys;

        u64 program;
        u64 index;
    };
}