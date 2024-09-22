#pragma once

#include <disk/virtual_types.h>
#include <disk/partition_filesystem.h>
#include <crypto/keysdb.h>
namespace peachnx::loader {
    class NSP {
    public:
        explicit NSP(const std::shared_ptr<crypto::KeysDb>& kdb, const disk::VirtFilePtr& nsp, u64 titleId, u64 programIndex);
        explicit NSP(const disk::VirtFilePtr& nsp);

        bool IsValidNsp() const;
        void ReadContent(const boost::unordered_map<std::string, std::shared_ptr<disk::OffsetFile>>& files) const;
        std::unique_ptr<disk::PartitionFilesystem> pfs;
    private:
        std::shared_ptr<crypto::KeysDb> keys;

        u64 program;
        u64 index;
    };
}