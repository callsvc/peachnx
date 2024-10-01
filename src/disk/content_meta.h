#pragma once

#include <disk/virtual_types.h>
namespace peachnx::disk {
    enum ContentType : u8 {
        Invalid,
        SystemProgram,
        SystemData,
        SystemUpdate,
        BootImagePackage, // Only used in firmware images
        BootImagePackageBd, // Also
        Application = 0x80,
        Patch,
        Addon,
        Delta,
        DataPatch // [15.0.0+]
    };

    enum ContentInfoType : u8 {
        Meta,
        Program,
        Data,
        Control,
        HtmlDoc,
        LegalInformation,
        DeltaFragment
    };
    enum MetaAttributes : u8 {
        None,
        IncludesExFatDriver,
        RebootLess,
        Compacted
    };

#pragma pack(push, 1)
    struct MetaHeader {
        u64 titleId;
        u32 version;
        ContentType metaType;
        u8 reserved0; // ContentMetaPlatform (NX == 0) Always 0 in version 17.0.0 or higher
        u16 extendedHeaderSize;
        u16 contentCount;
        u16 contentMetaCount; // Must be less than the available number of NCAs
        u8 attributes;
        std::array<u8, 0x3> reserved1;
        u32 requiredVersion;
        u32 reserved2;
    };

    struct ContentInfo {
        std::array<u8, 0x20> hash;
        std::array<u8, 0x10> contentId;
        std::array<u8, 0x6> size;
        ContentInfoType infoType;
        u8 reserved;
    };
    static_assert(sizeof(ContentInfo) == 0x38);

    struct MetaInfo {
        u8 id;
        u32 version;
        ContentType type;
        MetaAttributes attributes;
        u16 reserved;
    };
#pragma pack(pop)

    class ContentMeta {
    public:
        explicit ContentMeta(const VirtFilePtr& cnmt);

        void ReadContent(const VirtFilePtr& cnmt);
        void ReadMeta(const VirtFilePtr& cnmt);

        auto GetTitleId() const {
            return header.titleId;
        }
        std::vector<ContentInfo> records;
        std::vector<MetaInfo> schemas;
    private:
        MetaHeader header;

        u64 recordsOffset;
    };
}