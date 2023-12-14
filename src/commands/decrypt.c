#include <stdio.h>

#include "cli/argtype.h"
#include "cli/subcommand.h"
#include "decrypt.h"

int DecryptAes128ECB(int argc, char* argv[]);

struct Subcommand DecryptCommands[] = {
    {"aes-128-ecb", "decrypt ciphertext", DecryptAes128ECB},
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
    struct Arg* key = Argtype_New(argv[1]);
    if (key != NULL) {
        if (key->buffer.length == 16) {
            struct Arg* cipher = Argtype_New(argv[2]);
            if (cipher != NULL) {
                if ((cipher->buffer.length % 16) == 0) {
                    puts("TODO: Implement aes encryption");
                    status = 0;
                } else {
                    fprintf(stderr, "Invalid ciphertext size. %zu %% 16 != 0\n", cipher->buffer.length);
                }
                Argtype_Free(cipher);
            }
        } else {
            fprintf(stderr, "Invalid key size. Got %zu bytes, expected 16.\n", key->buffer.length);
        }
        Argtype_Free(key);
    }
    return status;
}
