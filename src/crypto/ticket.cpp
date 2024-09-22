#include <boost/align/align_up.hpp>

#include <crypto/ticket.h>
namespace peachnx::crypto {
    enum SignatureType : u32 {
        RSA_4096_SHA_1 = 0x010000,
        RSA_2048_SHA_1,
        ECDSA_SHA_1,
        RSA_4096_SHA_256,
        RSA_2048_SHA_256,
        ECDSA_SHA_256,
        HMAC_SHA1_160
    };
    Ticket::Ticket(const disk::VirtFilePtr& ticket) {
        const auto signature{ticket->Read<SignatureType>()};
        u32 offset{};
        if (signature == HMAC_SHA1_160) {
            throw std::runtime_error("HMAC_SHA1_160 is not used by the platform");
        }
        // We'll skip the signatures for now
        switch (signature) {
            case RSA_4096_SHA_1:
            case RSA_4096_SHA_256:
                offset = 0x240;
                break;
            case RSA_2048_SHA_1:
            case RSA_2048_SHA_256:
                offset = 0x140;
                break;
            case ECDSA_SHA_1:
            case ECDSA_SHA_256:
                offset = 0x80;
                break;
            default:
                offset = std::numeric_limits<u32>::max();
        }
        if (offset > 0x240) {
            throw std::runtime_error("Invalid signature offset");
        }

        offset = boost::alignment::align_up(offset, 0x40);

        *this = ticket->Read<Ticket>(offset);
        if (ticketVersion0 != 2) {
        }
    }

}