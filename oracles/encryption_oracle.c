#include <stddef.h>

#include <openssl/evp.h>
#include <openssl/rand.h>

#include "cli/argtype.h"
#include "operations/aes.h"
#include "parsers/base64.h"
#include "types/buffer.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fputs("Nothing to encrypt!\n", stderr);
        return EXIT_FAILURE;
    }

    bool success = false;
    uint8_t blocksize = 16;
    struct Buffer* plaintext = Argtype_New(argv[1]);
    struct Buffer* key = Buffer_New(blocksize);
    struct Buffer* iv = Buffer_New(blocksize);
    struct Buffer* ciphertext = NULL;
    uint8_t use_ecb;

    // Only continue if memory allocation succeeded
    if (plaintext && key && iv) {
        // Proceed if randomness is generated
        if (RAND_priv_bytes(key->data, blocksize) &&
            RAND_priv_bytes(iv->data, blocksize) &&
            RAND_bytes(&use_ecb, 1)) {
            // encrypt the plaintext
            if (use_ecb & 1) {
                ciphertext = Aes128Ecb_Encrypt(key, plaintext);
            } else {
                ciphertext = Aes128Cbc_Encrypt(key, iv, plaintext);
            }
            // Print the result
            if (ciphertext) {
                struct Buffer* b64 = Base64_Encode(ciphertext->data, ciphertext->length);
                if (b64) {
                    printf("%s\n", b64->data);
                    Buffer_Free(b64);
                    success = true;
                }
            }
        } else {
            fputs("Failed to generate randomness!\n", stderr);
        }
    } else {
        fputs("Memory allocation failed\n", stderr);
    }

    Argtype_Free(plaintext);
    Buffer_Free(key);
    Buffer_Free(iv);
    Buffer_Free(ciphertext);

    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}