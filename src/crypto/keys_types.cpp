#include <boost/algorithm/string/predicate.hpp>

#include <crypto/keys_types.h>
namespace Peachnx::Crypto {

    bool KeyIsEmpty(const Key128& key) {
        return boost::algorithm::equals(key, Key128{});
    }
}
