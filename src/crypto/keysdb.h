#pragma once

#include <types.h>
#include <unordered_map>

#include <crypto/ticket.h>

namespace peachnx::core {
    class AssetsBacking;
}
namespace peachnx::crypto {
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

        std::optional<Key256> headerKey{};
    private:
        void ParserKeyFile(const disk::VirtFilePtr& keyFile, KeyType type);
        std::unordered_map<std::string_view, std::optional<Key256>> keys256Id{};
        std::unordered_map<std::string, Key128> productionKeys{};

        std::unordered_map<Key128, Key128, Key128Hash> titleKeys{};
        std::unordered_map<std::string, std::unordered_map<u64, Key128>> indexedKeyNames{};
        std::unordered_map<Key128, Ticket, Key128Hash> tickets{};
    };
}