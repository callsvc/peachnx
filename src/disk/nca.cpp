#include <disk/nca.h>

#include <crypto/aes_storage.h>
namespace peachnx::disk {
    NCA::NCA(const crypto::KeysDb& keysDb, const VirtFilePtr& nca) {
        if (!nca)
            return;
        header = nca->Read<NcaHeader>();
        if (header.magic != MakeMagic<u32>("NCA3")) {
            if (!keysDb.headerKey)
                throw std::runtime_error("Content key inaccessible");
            auto titleKey{*keysDb.headerKey};
            crypto::AesStorage storage(MBEDTLS_CIPHER_AES_128_XTS, titleKey);
            storage.DecryptXts<NcaHeader>(header, 0, 0x200);
        }
    }
}
