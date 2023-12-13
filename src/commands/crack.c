#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "analyzers/english.h"
#include "cli/argtype.h"
#include "cli/subcommand.h"
#include "crackers/cracksbx.h"
#include "crackers/crackrbx.h"
#include "operations/xor.h"
#include "crack.h"

int CrackCli_SingleByteXor(int argc, char* argv[]);
int CrackCLI_RepeatingXor(int argc, char* argv[]);

struct Subcommand CrackCommands[] = {
    {"sbx", "Crack single byte xor cipher", CrackCli_SingleByteXor},
    {"rbx", "Crack repeating xor cipher", CrackCLI_RepeatingXor},
    {NULL}
};

void CrackUsage() {
    puts("The crack utility implements some encryption cracking techniques\n"
         "\n"
         "Commands:");
    Subcommand_PrintHelp(CrackCommands);
}

int CrackCli(int argc, char* argv[]) {
    if (argc > 1) {
        int result = Subcommand_Run(argv[1], CrackCommands, argc - 1, &argv[1]);
        if (result == SUBCOMMAND_NOT_FOUND) {
            fputs("Invalid command. See help below\n\n", stderr);
            CrackUsage();
            return EXIT_FAILURE;
        }
        return result;
    }
    CrackUsage();
    return 0;
}

int CrackCli_SingleByteXor(int argc, char* argv[]) {
    bool verbose = false;
    char* data = NULL;
    if (argc >= 2) {
        data = argv[1];
    }
    if (argc == 3) {
        if (strncmp(argv[1], "-v", 2) == 0) {
            verbose = true;
            data = argv[2];
        }
    }

    if (data == NULL) {
        puts("Usage:");
        puts("  sbx [-v] ciphertext");
        puts("");
        puts("Options:");
        puts("  -v    Create a file for every key in the current directory");
        puts("");
        puts("crack sbx will crack a single byte xor cipher.");
        puts("There are only 255 possible ciphertexts for a message");
        puts("that has been 'encrypted' with this method.");
        puts("crack sbx will brute force the cipher and perform");
        puts("frequency analysis on the results to choose the");
        puts("plaintext which seems the most like english plaintext.");
        puts("Pull requests to support other languages are encouraged");
        puts("");
        puts("If the crack fails, you can try the -v option.");
        puts("This will create 256 files in the current directory.");
        puts("Inside each file will be the cipher xor'd with the titular key.");
        puts("From there you can inspect the xor'd 'plaintexts' manually.");
        return EXIT_FAILURE;
    } else {
        struct Arg* arg = Argtype_New(data);
        if (arg != NULL) {
            struct PotentialKeys keys = CrackSBX(&arg->buffer, English_Analyzer, verbose);
            printf("Potential Keys:\n");
            struct Buffer* pt = Buffer_New(arg->buffer.length);
            if (pt != NULL) {
                for (int i = 0; i < 5; i++) {
                    if (sb_xor(pt, keys.keys[i], &arg->buffer) == XOR_SUCCESS) {
                        if (pt != NULL) {
                            printf("  0x%02X | Score %.02f | Message: %s\n", keys.keys[i], keys.scores[i], pt->data);
                        } else {
                            fputs("Failed to allocate memory?", stderr);
                        }
                    }
                }
                Buffer_Free(pt);
            }
            Argtype_Free(arg);
        }
    }
    return 0;
}

int CrackCLI_RepeatingXor(int argc, char* argv[]) {
    if ((argc < 2) || (argc > 4)) {
        puts("Usage:\n"
             "  rbx <ciphertext> [min key length] [max key length]\n"
             "\n"
             "Attempts to crack a repeating xor cipher and returns key values.\n"
             "You can specify the minimum and maximum key lengths to test.\n"
             "By default, min key length is 2 and max is 40");
        return EXIT_FAILURE;
    }
    size_t keymin = 2;
    size_t keymax = 40;
    if (argc > 2) {
        long long user_keymin = atoll(argv[2]);
        if (user_keymin <= 0) {
            fputs("Invalid minimum key length", stderr);
            return EXIT_FAILURE;
        }
        keymin = (size_t) user_keymin;
    }
    if (argc > 3) {
        long long user_keymax = atoll(argv[3]);
        if ((user_keymax <= 0) || (((size_t) user_keymax) < keymin)) {
            fputs("Invalid maximum key length", stderr);
            return EXIT_FAILURE;
        }
        keymax = (size_t) user_keymax;
    }

    struct Arg* arg = Argtype_New(argv[1]);
    if (arg != NULL) {
        struct RepeatingXorKeys* keys = CrackRepeatingXor(keymin, keymax, &arg->buffer, English_Analyzer, true);
        CrackRBX_FreeKeys(keys);
        Argtype_Free(arg);
    } else {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
