#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <ranges>

#include <boost/algorithm/hex.hpp>
#include <boost/algorithm/string.hpp>

#include <core/assets_backing.h>
#include <crypto/keysdb.h>
namespace peachnx::crypto {
    template <u64 Size>
    auto StringToByteArray(const std::string_view& desired) {
        namespace bad = boost::algorithm::detail;
        std::array<u8, Size> result{};
        if (desired.size() / 2 != Size)
            return result;
        u32 resIndex{};
        for (decltype(Size) index{}; index < desired.size(); index += 2) {
            result[resIndex] = bad::hex_char_to_int(desired[index]) << 4;
            result[resIndex] |= bad::hex_char_to_int(desired[index + 1]) & 0xf;
            resIndex++;
        }

        return result;
    }

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
    void KeysDb::Initialize(const core::AssetsBacking& assets) {
        const auto prod{assets.GetProdKey()};
        const auto title{assets.GetTitleKeys()};
        if (exists(prod))
            ParserKeyFile(std::make_shared<disk::RegularFile>(prod), KeyType::Production);
        if (exists(title))
            ParserKeyFile(std::make_shared<disk::RegularFile>(title), KeyType::Title);

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
            const auto keyIndex = [&] -> u64 {
                for (const auto& [index, value] : std::views::enumerate(indexedKeys)) {
                    if (keyIndexName == value)
                        return index;
                }
                return -1;
            }();
            if (keyIndex == -1)
                return;
            try {
                const KeyIndexPair pair{boost::lexical_cast<u32>(name.substr(keyIndexName.size(), 2)), keyIndex};
                indexedIt->second[pair] = StringToByteArray<16>(value);
            } catch ([[maybe_unused]] boost::bad_lexical_cast& be) {
            }
        }
    }
    std::optional<Key128> KeysDb::GetKey(const u64 master, const u64 index) const {
        const KeyIndexPair keyPair{master, index};
        for (const auto& indexes: std::views::values(indexedKey128Names)) {
            if (indexes.contains(keyPair))
                if (!KeyIsEmpty(indexes.at(keyPair)))
                    return indexes.at(keyPair);
        }
        return std::nullopt;
    }

    std::optional<Key128> KeysDb::GetKey(const std::string_view& kind, const u64 master, const u64 index) const {
        const KeyIndexPair keyPair{master, index};
        auto indexedIt{indexedKey128Names.begin()};
        for (; indexedIt != std::end(indexedKey128Names); ++indexedIt ) {
            if (indexedIt->first.find(kind) != std::string::npos)
                break;
        }
        if (indexedIt != std::end(indexedKey128Names))
            if (indexedIt->second.contains(keyPair))
                return indexedIt->second.at(keyPair);
        return {};
    }
    std::optional<Key128> KeysDb::GetTitle(const Key128& title) const {
        const auto desired{titleKeys.find(title)};
        if (desired != std::end(titleKeys))
            if (!KeyIsEmpty(desired->second))
                return desired->second;

        return std::nullopt;
    }

    void KeysDb::ParserKeyFile(const disk::VirtFilePtr& keyFile, const KeyType type) {
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
