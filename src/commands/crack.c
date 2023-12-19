#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "analyzers/english.h"
#include "cli/argtype.h"
#include "cli/flags.h"
#include "cli/subcommand.h"
#include "crackers/cracksbx.h"
#include "crackers/crackrbx.h"
#include "crackers/ecb_leak.h"
#include "operations/xor.h"
#include "crack.h"

int CrackCli_SingleByteXor(int argc, char* argv[]);
int CrackCLI_RepeatingXor(int argc, char* argv[]);
int CrackCLI_EcbLeak(int argc, char* argv[]);

struct Subcommand CrackCommands[] = {
    {"sbx", "Crack single byte xor cipher", CrackCli_SingleByteXor},
    {"rbx", "Crack repeating xor cipher", CrackCLI_RepeatingXor},
    {"ecbleak", "Tap a program using ecb encryption", CrackCLI_EcbLeak},
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
        struct Buffer* arg = Argtype_New(data);
        if (arg != NULL) {
            struct PotentialKeys keys = CrackSBX(arg, English_Analyzer, verbose);
            printf("Potential Keys:\n");
            struct Buffer* pt = Buffer_New(arg->length);
            if (pt != NULL) {
                for (int i = 0; i < 5; i++) {
                    if (sb_xor(pt, keys.keys[i], arg) == XOR_SUCCESS) {
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
    if ((argc < 2) || (argc > 5)) {
        puts("Usage:\n"
             "  rbx [-v] <ciphertext> [min key length] [max key length]\n"
             "\n"
             "Attempts to crack a repeating xor cipher and returns key values.\n"
             "You can specify the minimum and maximum key lengths to test.\n"
             "By default, min key length is 2 and max is 40\n"
             "\n"
             "Options:\n"
             "  -v    verbose, print extra information");
        return EXIT_FAILURE;
    }
    bool verbose = false;
    size_t keymin = 2;
    size_t keymax = 40;
    int ciphertext_index = 1;
    if (strncmp(argv[ciphertext_index], "-v", 2) == 0) {
        verbose = true;
        ciphertext_index += 1;
    }
    int keymin_index = ciphertext_index + 1;
    if (keymin_index < argc) {
        long long user_keymin = atoll(argv[ciphertext_index + 1]);
        if (user_keymin <= 0) {
            fputs("Invalid minimum key length", stderr);
            return EXIT_FAILURE;
        }
        keymin = (size_t) user_keymin;
    }

    int keymax_index = ciphertext_index + 2;
    if (keymax_index < argc) {
        long long user_keymax = atoll(argv[ciphertext_index + 2]);
        if ((user_keymax <= 0) || (((size_t) user_keymax) < keymin)) {
            fputs("Invalid maximum key length", stderr);
            return EXIT_FAILURE;
        }
        keymax = (size_t) user_keymax;
    }

    struct Buffer* arg = Argtype_New(argv[ciphertext_index]);
    if (arg != NULL) {
        struct RepeatingXorKeys* keys = CrackRepeatingXor(keymin, keymax, arg, English_Analyzer, verbose);
        // CrackRepeatingXor prints the summary already if verbose is true
        if (!verbose) {
            CrackRBX_PrintSummary(keys);
        }
        CrackRBX_FreeKeys(keys);
        Argtype_Free(arg);
    } else {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int CrackCLI_EcbLeak(int argc, char* argv[]) {
    (void)argv;
    if (argc < 2) {
        puts(
            "Usage:\n"
            "  ecbleak [-b] \"command ...FUZZ...\"\n"
            "\n"
            "ecbleak attempts to leak data from an application using ecb encryption.\n"
            "command must be a program or script which returns encrypted data either\n"
            "as raw binary data, or base64 encoded data\n"
            "If you're testing a web application, it could be a script which\n"
            "sends the HTTP request, extracts the ciphertext and prints it to stdout\n"
            "\n"
            "A vulnerable application will append some internal data to your user input\n"
            "and encrypt the data via aes-128-ecb mode encryption. That encrypted data\n"
            "must be written to stdout.\n"
            "and ecbleak will read it from there\n"
            "\n"
            "Command arguments should contain the word FUZZ, this is the section of args\n"
            "that ecbleak will modify to try to get the application to leak data."
            "\n"
            "Options:\n"
            "  -b    Command writes binary data"
        );
        return EXIT_FAILURE;
    }
    char* cmd = argv[1];
    bool binary = false;
    (void)binary;
    if (FlagExists("-b", argc, argv)) {
        cmd += 1;
        binary = true;
    }
    struct Buffer* leaked = ECBLeak(cmd, true, true);
    Buffer_Free(leaked);
    return EXIT_SUCCESS;
}
