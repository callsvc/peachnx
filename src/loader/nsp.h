#pragma once

#include <crypto/keysdb.h>
#include <disk/partition_filesystem.h>

#include <disk/nca.h>
namespace peachnx::loader {
    class NSP {
    public:
        explicit NSP(const std::shared_ptr<crypto::KeysDb>& kdb, const disk::VirtFilePtr& nsp, u64 titleId, u64 programIndex);
        explicit NSP(const disk::VirtFilePtr& nsp);

        bool IsValidNsp() const;
        void ReadContent(const boost::unordered_map<std::string, disk::VirtFilePtr>& files);

        std::vector<std::unique_ptr<disk::NCA>> contents;
    private:
        std::unique_ptr<disk::PartitionFilesystem> pfs;
        std::shared_ptr<crypto::KeysDb> keys;

        u64 program;
        u64 index;
    };
}