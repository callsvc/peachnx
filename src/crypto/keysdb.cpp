#include <crypto/keysdb.h>
namespace peachnx::crypto {
    KeysDb::KeysDb() {
        keys256Id["header_key"] = {};
        keys256Id["sd_card_save_key_source"] = {};
        keys256Id["sd_card_nca_key_source"] = {};
        keys256Id["header_key_source"] = {};
        keys256Id["sd_card_save_key"] = {};
        keys256Id["sd_card_nca_key"] = {};
    }

    void KeysDb::AddTicket(const Ticket& ticket) {
        if (!ticket.ticketKeyType)
            tickets.emplace(ticket.rightsId, ticket);
    }
    void KeysDb::AddTitleKey(const Key128 key, const Key128 value) {
        if (title.contains(key))
            title.emplace(key, value);
    }
    void KeysDb::AddTitleKey(const std::string_view keyName, std::variant<Key128, Key256> value) {
        if (keys256Id.contains(keyName)) {
            keys256Id[keyName] = headerKey = std::get<Key256>(value);
        }

        if (keyName.size()) {
            std::stringstream hex;
        }
    }
}
