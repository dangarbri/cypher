#include <openssl/evp.h>

#include "aes.h"

struct Buffer* Aes128Ecb(struct Buffer* key, struct Buffer* data, bool encrypt) {
    if (!Buffer_IsValid(key) || !Buffer_IsValid(data) || (key->length != 16) || ((data->length % 16) != 0)) {
        return NULL;
    }

    bool failure = true;
    // Extra length is just in case openssl does anything strange in EncryptFinal_Ex...
    struct Buffer* result = Buffer_New(data->length + key->length);
    if (result == NULL) {
        return NULL;
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    // bool is implementation defined, and openssl wants a 1 or 0.
    int do_encrypt = encrypt ? 1 : 0;
    if (ctx != NULL) {
        // if do_encrypt == 1, encryption is performed, if 0 then decryption is performed. else behavior is undefined (to me).
        if (EVP_CipherInit_ex2(ctx, EVP_aes_128_ecb(), key->data, NULL, do_encrypt, NULL) == 1) {
            int outl;
            if (EVP_CipherUpdate(ctx, result->data, &outl, data->data, (int) data->length) == 1) {
                int tmplen;
                if (EVP_CipherFinal_ex(ctx, result->data + outl, &tmplen) == 1) {
                    result->length = outl + tmplen;
                    failure = false;
                }
            }
        } else {
            fputs("Failed to initialize encryption envelope\n", stderr);
        }

        EVP_CIPHER_CTX_free(ctx);
    }

    if (failure) {
        Buffer_Free(result);
        result = NULL;
    }
    return result;
}

struct Buffer* Aes128Ecb_Encrypt(struct Buffer* key, struct Buffer* data) {
    return Aes128Ecb(key, data, true);
}

struct Buffer* Aes128Ecb_Decrypt(struct Buffer* key, struct Buffer* data) {
    return Aes128Ecb(key, data, false);
}