#include <stdio.h>

#include "cli/argtype.h"
#include "cli/subcommand.h"
#include "operations/aes.h"
#include "decrypt.h"

int DecryptAes128ECB(int argc, char* argv[]);
int DecryptAes128CBC(int argc, char* argv[]);

struct Subcommand DecryptCommands[] = {
    {"aes-128-ecb", "decrypt ciphertext", DecryptAes128ECB},
    {"aes-128-cbc", "decrypt ciphertext", DecryptAes128CBC},
    {NULL}
};

void DecryptPrintHelp() {
    puts("Decryption utilities\n"
         "\n"
         "COMMANDS:");
    Subcommand_PrintHelp(DecryptCommands);
}

int Decrypt(int argc, char* argv[]) {
    if (argc <= 1) {
        DecryptPrintHelp();
        return 1;
    } else {
        int result = Subcommand_Run(argv[1], DecryptCommands, argc - 1, &argv[1]);
        if (result == SUBCOMMAND_NOT_FOUND) {
            fprintf(stderr, "Invalid command '%s'\n", argv[1]);
        }
        return result;
    }
}

int DecryptAes128ECB(int argc, char* argv[]) {
    if (argc != 3) {
        puts("Usage:\n"
             "  aes-128-ecb <key> <ciphertext>\n"
             "\n"
             "Decrypts some aes-128-ecb encrypted ciphertext.\n"
             "I have nothing more to say about that.");
        return 1;
    }
    int status = 1;
    struct Buffer* key = Argtype_New(argv[1]);
    if (key != NULL) {
        if (key->length == 16) {
            struct Buffer* cipher = Argtype_New(argv[2]);
            if (cipher != NULL) {
                if ((cipher->length % 16) == 0) {
                    struct Buffer* result = Aes128Ecb_Decrypt(key, cipher);
                    if (result != NULL) {
                        fwrite(result->data, 1, result->length, stdout);
                        fflush(stdout);
                        Buffer_Free(result);
                        status = 0;
                    } else {
                        fputs("Failed to decrypt the ciphertext\n", stderr);
                    }
                } else {
                    fprintf(stderr, "Invalid ciphertext size. %zu %% 16 != 0\n", cipher->length);
                }
                Argtype_Free(cipher);
            }
        } else {
            fprintf(stderr, "Invalid key size. Got %zu bytes, expected 16.\n", key->length);
        }
        Argtype_Free(key);
    }
    return status;
}

int DecryptAes128CBC(int argc, char* argv[]) {
    if (argc != 4) {
        puts("Usage:\n"
             "  aes-128-cbc <key> <iv> <ciphertext>\n"
             "\n"
             "Decrypts some aes-128-cbc encrypted ciphertext.\n"
             "I have nothing more to say about that.");
        return 1;
    }
    int status = 1;
    struct Buffer* key = Argtype_New(argv[1]);
    if (key != NULL) {
        if (key->length == 16) {
            struct Buffer* iv = Argtype_New(argv[2]);
            if (iv != NULL) {
                if (iv->length == 16) {
                    struct Buffer* cipher = Argtype_New(argv[3]);
                    if (cipher != NULL) {
                        if ((cipher->length % 16) == 0) {
                            struct Buffer* result = Aes128Cbc_Decrypt(key, iv, cipher);
                            if (result != NULL) {
                                fwrite(result->data, 1, result->length, stdout);
                                fflush(stdout);
                                Buffer_Free(result);
                                status = 0;
                            } else {
                                fputs("Failed to decrypt the ciphertext\n", stderr);
                            }
                        } else {
                            fprintf(stderr, "Invalid ciphertext size. %zu %% 16 != 0\n", cipher->length);
                        }
                        Argtype_Free(cipher);
                    }
                } else {
                    fprintf(stderr, "Invalid IV length, got %zu bytes, expected 16\n", iv->length);
                }
            } else {
                fputs("Couldn't parse your IV arg\n", stderr);
            }
        } else {
            fprintf(stderr, "Invalid key size. Got %zu bytes, expected 16.\n", key->length);
        }
        Argtype_Free(key);
    } else {
        fputs("Couldn't parse your key arg\n", stderr);
    }
    return status;
}

