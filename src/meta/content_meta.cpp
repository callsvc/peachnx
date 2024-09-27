#include <cassert>

#include <meta/content_meta.h>
namespace peachnx::meta {
    ContentMeta::ContentMeta(const disk::VirtFilePtr& cnmt) {
        header = cnmt->Read<MetaHeader>();

        assert(header.contentMetaCount < header.contentCount);

        recordsOffset = header.extendedHeaderSize + sizeof(MetaHeader);
        if (header.contentCount)
            ReadContent(cnmt);
        if (header.contentMetaCount)
            ReadMeta(cnmt);
    }
    void ContentMeta::ReadContent(const disk::VirtFilePtr& cnmt) {
        records.resize(header.contentCount);
        for (u32 rec{}; rec < records.size(); rec++) {
            const auto offset{recordsOffset + rec * sizeof(ContentInfo)};
            if (cnmt->GetSize() < offset + sizeof(ContentInfo)) {
                records.resize(rec);
                break;
            }
            records[rec] = cnmt->Read<ContentInfo>(offset);
        }
    }
    void ContentMeta::ReadMeta(const disk::VirtFilePtr& cnmt) {
        schemas.resize(header.contentMetaCount);
        u32 rec{};
        for (; rec < schemas.size(); rec++) {
            const auto offset{recordsOffset + rec * sizeof(MetaInfo)};
            if (cnmt->GetSize() < offset + sizeof(MetaInfo))
                break;
            schemas[rec] = cnmt->Read<MetaInfo>(offset);
        }
        schemas.resize(rec);
    }
}
