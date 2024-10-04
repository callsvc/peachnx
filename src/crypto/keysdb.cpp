#include <algorithm>
#include <ranges>

#include <boost/algorithm/string.hpp>

#include <generic.h>
#include <core/assets_backing.h>
#include <crypto/keysdb.h>
namespace Peachnx::Crypto {
    static constexpr std::array keys {
        "header_key", "sd_card_save_key_source", "sd_card_nca_key_source", "sd_card_nca_key_source", "header_key_source", "sd_card_save_key", "sd_card_nca_key"
    };
    static constexpr std::array indexedKeys {
        "key_area_key_application_", "key_area_key_ocean_", "key_area_key_system_", "titlekek_"
    };

    KeysDb::KeysDb() {
        for (const auto& keysName : keys) {
            keys256Id.emplace(keysName, std::nullopt);
        }
        for (const auto& indexed : indexedKeys) {
            indexedKey128Names.emplace(indexed, IndexKey128{});
        }
    }
    void KeysDb::Initialize(const Core::AssetsBacking& assets) {
        const auto prod{assets.GetProdKey()};
        const auto title{assets.GetTitleKeys()};
        if (exists(prod))
            ParserKeyFile(std::make_shared<SysFs::RegularFile>(prod), KeyType::Production);
        if (exists(title))
            ParserKeyFile(std::make_shared<SysFs::RegularFile>(title), KeyType::Title);

        if (titleKeys.empty()) {
            throw std::runtime_error("Empty keys file");
        }
    }

    void KeysDb::AddTicket(const Ticket& ticket) {
        if (!ticket.ticketKeyType)
            tickets.emplace(ticket.rightsId, ticket);
    }
    void KeysDb::AddTitleKey(Key128 key, Key128 value) {
        if (!titleKeys.contains(key))
            titleKeys.emplace(key, value);
    }
    void KeysDb::AddTitleKey(const std::string_view& name, const std::string_view& value) {
        const Key128 key{StringToByteArray<16>(name)};
        const Key128 right{StringToByteArray<16>(value)};
        AddTitleKey(key, right);
    }
    void KeysDb::AddProdKey(const std::string_view& name, const std::string_view& value) {
        const auto keysIt{keys256Id.find(name)};
        if (keysIt != std::end(keys256Id)) {
            const auto taggedKey{StringToByteArray<32>(value)};
            if (keysIt->first == keys[0])
                headerKey = taggedKey;
            keysIt->second = taggedKey;

            return;
        }

        const auto keyIndexName{name.substr(0, name.size() - 2)};
        const auto indexedIt{indexedKey128Names.find(keyIndexName)};

        if (indexedIt != std::end(indexedKey128Names)) {
            try {
                std::stringstream vs;
                vs << std::hex << name.substr(keyIndexName.size(), 2);

                u32 index{};
                vs >> index;
                indexedIt->second[index] = StringToByteArray<16>(value);
            } catch ([[maybe_unused]] std::exception& be) {
                __builtin_trap();
            }
        }
    }

    std::optional<Key128> KeysDb::GetKey(const IndexedKey128Type indexed, const u64 generation, const u64 type) const {
        auto indexedIt{indexedKey128Names.begin()};
        for (; indexedIt != std::end(indexedKey128Names); ++indexedIt) {
            if (indexedIt->first.find(indexedKeys[indexed]) != std::string::npos)
                break;
        }

        if (indexedIt != std::end(indexedKey128Names)) {
            if (indexed != Titlekek)
                assert(indexed == type);
            if (indexedIt->second.contains(generation))
                if (!KeyIsEmpty(indexedIt->second.at(generation)))
                    return indexedIt->second.at(generation);
        }
        return {};
    }
    std::optional<Key128> KeysDb::GetTitle(const Key128& title) const {
        const auto desired{titleKeys.find(title)};
        if (desired != std::end(titleKeys))
            if (!KeyIsEmpty(desired->second))
                return desired->second;

        return std::nullopt;
    }

    void KeysDb::ParserKeyFile(const SysFs::VirtFilePtr& keyFile, const KeyType type) {
        const auto content{keyFile->GetBytes(keyFile->GetSize())};
        if (content.empty())
            throw std::runtime_error("Could not parse key file");

        std::vector<std::string> pairs;
        split(pairs, content, boost::is_any_of("\n"));
        if (pairs.size() % 2)
            pairs.pop_back();

        for (const auto& line : pairs) {
            if (line.size() > 1024)
                continue;

            auto keyName{line.substr(0, line.find('='))};
            auto keyValue{line.substr(line.find('=') + 1)};

            boost::trim(keyName);
            boost::trim(keyValue);

            if (type == KeyType::Production)
                AddProdKey(keyName, keyValue);
            else
                AddTitleKey(keyName, keyValue);
        }
    }
}
