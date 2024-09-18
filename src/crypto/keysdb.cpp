#include <boost/algorithm/string.hpp>

#include <crypto/keysdb.h>
#include <core/assets_backing.h>
namespace peachnx::crypto {
    template <u64 Size>
    auto StringToByteArray(const std::string_view& desired) {
        auto toByte = [] (const auto str) -> u8 {
            if (str >= 'A' && str <= 'F')
                return str - 'A' + 10;
            if (str >= 'a' && str <= 'f')
                return str - 'a' + 10;

            if (str >= '0' && str <= '9')
                return str - '0';
            throw std::runtime_error("Invalid string");
        };
        std::array<u8, Size> result{};
        if (desired.size() / 2 != Size)
            return result;
        u32 resIndex{};
        for (decltype(Size) index{}; index < desired.size(); index += 2) {
            result[resIndex] = toByte(desired[index]) << 4;
            result[resIndex] |= toByte(desired[index + 1]) & 0xf;
            resIndex++;
        }

        return result;
    }

    static constexpr std::array keys {
        "header_key", "sd_card_save_key_source", "sd_card_nca_key_source", "sd_card_nca_key_source", "header_key_source", "sd_card_save_key", "sd_card_nca_key"
    };

    KeysDb::KeysDb() {
        for (const auto& keysName : keys) {
            keys256Id.emplace(keysName, std::nullopt);
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
        }

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
