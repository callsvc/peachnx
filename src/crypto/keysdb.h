#pragma once

#include <types.h>
#include <unordered_map>

#include <crypto/ticket.h>
namespace peachnx::core {
    class AssetsBacking;
}
namespace peachnx::crypto {
    struct KeyIndexPair {
        u64 field0;
        u64 field1;

        auto operator==(const KeyIndexPair& key) const {
            return field0 == key.field0 && field1 == key.field1;
        }
    };

    struct KeyIndexHash {
        auto operator()(const KeyIndexPair& key) const {
            return std::hash<u64>()(key.field0) ^ std::hash<u64>()(key.field1);
        }
    };

    using IndexKey128 = std::unordered_map<KeyIndexPair, Key128, KeyIndexHash>;

    enum class KeyType {
        Production,
        Title
    };

    class KeysDb {
    public:
        KeysDb();
        void Initialize(const core::AssetsBacking& assets);
        void AddTicket(const Ticket& ticket);
        void AddTitleKey(Key128 key, Key128 value);
        void AddTitleKey(const std::string_view& name, const std::string_view& value);
        void AddProdKey(const std::string_view& name, const std::string_view& value);

        bool Exists(u64 master, u64 index) const;

        std::optional<Key256> headerKey{};
    private:
        void ParserKeyFile(const disk::VirtFilePtr& keyFile, KeyType type);
        std::unordered_map<std::string_view, std::optional<Key256>> keys256Id{};
        std::unordered_map<std::string, Key128> productionKeys{};

        std::unordered_map<Key128, Key128, Key128Hash> titleKeys{};
        std::unordered_map<std::string_view, IndexKey128> indexedKey128Names{};
        std::unordered_map<Key128, Ticket, Key128Hash> tickets{};
    };
}