#pragma once

#include <types.h>
#include <unordered_map>

#include <crypto/ticket.h>
namespace peachnx::crypto {
    class KeysDb {
    public:
        KeysDb();
        void AddTicket(const Ticket& ticket);
        void AddTitleKey(Key128 key, Key128 value);
        void AddTitleKey(std::string_view keyName, std::variant<Key128, Key256> value);

        std::optional<Key256> headerKey{};
    private:
        std::unordered_map<std::string_view, std::optional<Key256>> keys256Id{};
        std::unordered_map<std::string, Key128> production{};

        std::unordered_map<Key128, Key128, Key128Hash> title{};
        std::unordered_map<std::string, std::unordered_map<u64, Key128>> indexedKeyNames{};
        std::unordered_map<Key128, Ticket, Key128Hash> tickets{};
    };
}