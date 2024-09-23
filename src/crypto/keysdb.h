#pragma once

#include <types.h>
#include <unordered_map>

#include <crypto/ticket.h>
namespace peachnx::core {
    class AssetsBacking;
}
namespace peachnx::crypto {
    using IndexKey128 = std::unordered_map<u64, Key128>;

    enum class KeyType {
        Production,
        Title
    };

    class KeysDb {
    public:
        KeysDb(const KeysDb&) = delete;

        KeysDb();
        void Initialize(const core::AssetsBacking& assets);
        void AddTicket(const Ticket& ticket);
        void AddTitleKey(Key128 key, Key128 value);
        void AddTitleKey(const std::string_view& name, const std::string_view& value);
        void AddProdKey(const std::string_view& name, const std::string_view& value);

        std::optional<Key128> GetKey(IndexedKey128Type indexed, u64 generation = {}, u64 type = {}) const;
        std::optional<Key128> GetTitle(const Key128& title) const;

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