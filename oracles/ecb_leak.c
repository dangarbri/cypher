#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli/argtype.h"
#include "operations/aes.h"
#include "parsers/base64.h"

const char* SECRET_MESSAGE = "Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkg\n"
"aGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBq\n"
"dXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUg\n"
"YnkK\n";

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fputs("Nothing to encrypt!\n", stderr);
        return EXIT_FAILURE;
    }

    int exit_status = EXIT_FAILURE;
    // Parse secret message
    struct Buffer* secret = Base64_Decode(SECRET_MESSAGE);
    if (secret) {
        // Load key
        char key_file[] = "file:ecb_key";
        struct Buffer* key = Argtype_New(key_file);
        if (key) {
            // get the buffer size
            size_t input_length = strlen(argv[1]);
            size_t secret_length = strlen((char*) secret->data);
            size_t bufsize = input_length + secret_length + 1;
            if (bufsize > secret_length) {
                // Concatenate secret message onto input
                struct Buffer* data = Buffer_New(bufsize);
                if (data) {
                    strcpy((char*) data->data, argv[1]);
                    strcat((char*) data->data, (char*) secret->data);
                    // Encrypt the data
                    struct Buffer* encrypted = Aes128Ecb_Encrypt(key, data);
                    if (encrypted) {
                        // Encode and print the data
                        struct Buffer* encoded = Base64_Encode(encrypted->data, encrypted->length);
                        if (encoded) {
                            printf("%s\n", encoded->data);
                            exit_status = EXIT_SUCCESS;
                            Buffer_Free(encoded);
                        } else {
                            fputs("Failed to b64 encode data\n", stderr);
                        }
                        Buffer_Free(encrypted);
                    } else {
                        fputs("Failed to encrypt data\n", stderr);
                    }
                    Buffer_Free(data);
                } else {
                    fputs("Couldn't allocate concat buffer\n", stderr);
                }
            } else {
                fputs("Overflow detected\n", stderr);
            }
            Buffer_Free(key);
        } else {
            fputs("Failed to load key. is ecb_key in the current dir?\n", stderr);
        }
        Buffer_Free(secret);
    } else {
        fputs("Failed to decode the secret message\n", stderr);
    }
    return exit_status;
}