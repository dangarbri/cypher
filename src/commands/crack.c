#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "analyzers/english.h"
#include "cli/argtype.h"
#include "cli/subcommand.h"
#include "crackers/cracksbx.h"
#include "operations/xor.h"
#include "crack.h"

int CrackCli_SingleByteXor(int argc, char* argv[]);

struct Subcommand CrackCommands[] = {
    {"sbx", "Crack single byte xor", CrackCli_SingleByteXor},
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
            struct Buffer buf = {
                .data = arg->data,
                .length = arg->length
            };
            struct PotentialKeys keys = CrackSBX(&buf, English_Analyzer, verbose);
            printf("Potential Keys:\n");
            for (int i = 0; i < 5; i++) {
                struct XorData pt = sb_xor(keys.keys[i], buf.data, buf.length);
                if (pt.data != NULL) {
                    printf("  0x%02X | Score %.02f | Message: %s\n", keys.keys[i], keys.scores[i], pt.data);
                    free(pt.data);
                } else {
                    fputs("Failed to allocate memory?", stderr);
                }
            }
            Argtype_Free(arg);
        }
    }
    return 0;
}