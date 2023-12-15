#include <string.h>
#include <openssl/evp.h>

#include "operations/pad.h"
#include "operations/xor.h"
#include "aes.h"

/**
 * @param key 16 byte Encryption key
 * @param data Data to process
 * @param encrypt true to encrypt data, false to decrypt data
 * @param pad true if data has padding/should be padded. false otherwise.
 */
struct Buffer* Aes128Ecb(struct Buffer* key, struct Buffer* data, bool encrypt, bool pad) {
    (void)pad;
    if (!Buffer_IsValid(key) || !Buffer_IsValid(data) || (key->length != 16)) {
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
            EVP_CIPHER_CTX_set_padding(ctx, pad ? 1 : 0);
            int outl;
            if (EVP_CipherUpdate(ctx, result->data, &outl, data->data, (int) data->length) == 1) {
                int tmplen;
                if (EVP_CipherFinal_ex(ctx, result->data + outl, &tmplen) == 1) {
                    result->length = outl + tmplen;
                    failure = false;
                }
            } else {
                fputs("EVP_CipherUpdate failed\n", stderr);
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
    return Aes128Ecb(key, data, true, true);
}

struct Buffer* Aes128Ecb_Decrypt(struct Buffer* key, struct Buffer* data) {
    // First try to decrypt with padding
    struct Buffer* plaintext = Aes128Ecb(key, data, false, true);
    if (plaintext == NULL) {
        // If that fails, try decrypt without padding
        plaintext = Aes128Ecb(key, data, false, false);
    }
    return plaintext;
}

struct Buffer* Aes128Cbc_Encrypt(struct Buffer* key, struct Buffer* iv, struct Buffer* data) {
    const uint8_t blocksize = 16;
    if (!Buffer_IsValid(key) || !Buffer_IsValid(iv) || !Buffer_IsValid(data) || (key->length != blocksize) || (iv->length != blocksize)) {
        return NULL;
    }
    if (Pad(data, blocksize, Pad_PKCS7) == 1) {
        fputs("Padding failed\n", stderr);
        return NULL;
    }

    struct Buffer* encrypted = Buffer_New(data->length);
    if (!Buffer_IsValid(encrypted)) {
        return NULL;
    }

    bool failure = true;
    struct Buffer* xor_block = Buffer_Clone(iv);
    if (Buffer_IsValid(xor_block)) {
        size_t num_blocks = data->length / blocksize;
        for (size_t block_id = 0; block_id < num_blocks; block_id++) {
            size_t offset = block_id * blocksize;
            // ptr to source plaintext block
            uint8_t* pblock = data->data + offset;
            // xor the plaintext with the previous block/iv
            // store the result in xor_block
            struct Buffer pt = {.data = pblock, .length = blocksize};
            if (xor(xor_block, xor_block, &pt) != XOR_SUCCESS) {
                break;
            }
            // aes encrypt it via ecb algo
            struct Buffer* ciphertext = Aes128Ecb(key, xor_block, 1, 0);
            if (ciphertext == NULL) {
                break;
            }

            // ptr to target encrypted block
            uint8_t* cblock = encrypted->data + offset;
            // Copy the result into the output buffer
            memcpy(cblock, ciphertext->data, blocksize);

            // swap the xor_block with the ciphertext so it gets xor'd with the next block.
            Buffer_Free(xor_block);
            xor_block = ciphertext;

            if (block_id == (num_blocks - 1)) {
                failure = false;
            }

        }
        Buffer_Free(xor_block);
    }

    if (failure) {
        Buffer_Free(encrypted);
        encrypted = NULL;
    }

    return encrypted;
}

struct Buffer* Aes128Cbc_Decrypt(struct Buffer* key, struct Buffer* iv, struct Buffer* data) {
    const uint8_t blocksize = 16;
    if (!Buffer_IsValid(key) || !Buffer_IsValid(iv) ||
        !Buffer_IsValid(data) || (key->length != blocksize)
        || (iv->length != blocksize) || ((data->length % blocksize) != 0)) {
        return NULL;
    }

    struct Buffer* decrypted = Buffer_New(data->length);
    if (!Buffer_IsValid(decrypted)) {
        return NULL;
    }

    bool failure = true;
    struct Buffer* xor_block = Buffer_New(blocksize);
    struct Buffer previous_block = { .data = iv->data, .length = blocksize};
    if (Buffer_IsValid(xor_block)) {
        size_t num_blocks = data->length / blocksize;
        for (size_t block_id = 0; block_id < num_blocks; block_id++) {
            size_t offset = block_id * blocksize;
            // ptr to source ciphertext block
            uint8_t* cipherblock = data->data + offset;
            // run the block through aes
            struct Buffer cipherbuffer = { .data = cipherblock, .length = blocksize };
            struct Buffer* plaintext = Aes128Ecb(key, &cipherbuffer, 0, 0);
            if (plaintext == NULL) {
                break;
            }

            // xor the plaintext with the previous block
            // store the result in xor_block
            enum XorResult xor_result = xor(xor_block, &previous_block, plaintext);
            Buffer_Free(plaintext);
            if (xor_result != XOR_SUCCESS) {
                break;
            }
            memcpy(decrypted->data + offset, xor_block->data, blocksize);
            previous_block.data = cipherblock;
            if (block_id == (num_blocks - 1)) {
                failure = false;
            }
        }
        Buffer_Free(xor_block);
    }

    if (failure) {
        Buffer_Free(decrypted);
        decrypted = NULL;
    }

    return decrypted;
}

