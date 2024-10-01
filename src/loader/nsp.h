#pragma once

#include <crypto/keysdb.h>
#include <disk/partition_filesystem.h>

#include <loader/nca.h>
#include <meta/content_meta.h>
namespace peachnx::loader {
    class NSP final : public Loader {
    public:
        explicit NSP(const std::shared_ptr<crypto::KeysDb>& kdb, const disk::VirtFilePtr& nsp, u64 titleId, u64 programIndex);
        explicit NSP(const disk::VirtFilePtr& nsp);

        static ApplicationType GetTypeFromFile(const disk::VirtFilePtr& probFile);

        void ReadContent(const boost::unordered_map<std::string, disk::VirtFilePtr>& files);

        std::vector<std::unique_ptr<NCA>> contents;
        std::vector<meta::ContentMeta> cnmts;

        // Stores all NCAs based on their title ID
        std::unordered_map<u64, u64> indexedNca;

        bool IsLoaded() const override;
    private:
        std::unique_ptr<disk::PartitionFilesystem> pfs;
        std::shared_ptr<crypto::KeysDb> keys;

        u64 program;
        u64 index;
    };
}