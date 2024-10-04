#include <boost/algorithm/string/predicate.hpp>
#include <crypto/checksum.h>

namespace Peachnx::Crypto {

    Checksum::Checksum(const std::array<u8, 32>& target) : match(target) {
        mbedtls_sha256_init(&context);
    }
    Checksum::~Checksum() {
        mbedtls_sha256_free(&context);
    }

    bool Checksum::MatchWith(const u8* data, const u64 size) {
        mbedtls_sha256_starts(&context, 0);

        mbedtls_sha256_update(&context, data, size);

        decltype(match) result;
        mbedtls_sha256_finish(&context, result.data());

        return boost::algorithm::equals(match, result);
    }
}
