#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "analyzers/english.h"
#include "cli/argtype.h"
#include "cli/subcommand.h"
#include "crackers/cracksbx.h"
#include "operations/xor.h"
#include "operations/sbxor.h"
#include "parsers/hex.h"
#include "types/buffer.h"
#include "xor_cli.h"

int sbx_cli_cmd(int argc, char* argv[]);
int sbx_cli_crack(int argc, char* argv[]);
int perform_xor(char* a, char* b);

struct Subcommand xor_commands[] = {
    {"sbx", "single byte xor operation", sbx_cli_cmd},
    {"cracksbx", "crack a single byte xor cipher", sbx_cli_crack},
    {NULL}
};

void xor_usage() {
    puts("Usage:");
    puts("  xor a b");
    puts("  xor hex:deadbeef four");
    puts("  xor base64:aGVsbG8= hello");
    puts("");
    puts("The xor command allows you to xor items on the command line");
    puts("Both arguments must be the same byte length after decoding.");
    puts("The output is printed as a string of hex characters");
    puts("");
    puts("More subcommands:");
    Subcommand_PrintHelp(xor_commands);
}

int perform_xor(char* a, char* b) {
    int ret = EXIT_FAILURE;
    struct Arg* left = Argtype_New(a);
    if (left != NULL) {
        struct Arg* right = Argtype_New(b);
        if (right != NULL) {
            if (left->length == right->length) {
                uint8_t* result = malloc(left->length);
                if (result != NULL) {
                    xor(result, left->data, right->data, left->length);
                    char* encoded = Hex_Encode(result, left->length);
                    if (encoded != NULL) {
                        printf("%s\n", encoded);
                        ret = EXIT_SUCCESS;
                        free(encoded);
                    } else {
                        fputs("Failed to hex encode xor result", stderr);
                    }
                    free(result);
                } else {
                    perror("xor_cli: ");
                }
            } else {
                fprintf(stderr, "Left and right arguments are not the same length (%zu != %zu)\n", left->length, right->length);
            }
            Argtype_Free(right);
        }
        Argtype_Free(left);
    }
    return ret;
}

int perform_sbx(char* a, char* b) {
    int ret = EXIT_FAILURE;
    struct Arg* left = Argtype_New(a);
    if (left != NULL) {
        struct Arg* right = Argtype_New(b);
        if (right != NULL) {
            if (left->length == 1) {
                struct XorData result = SBX_Perform(*left->data, right->data, right->length);
                if (result.data != NULL) {
                    char* encoded = Hex_Encode(result.data, result.length);
                    if (encoded != NULL) {
                        printf("%s\n", encoded);
                        ret = EXIT_SUCCESS;
                        free(encoded);
                    }
                    free(result.data);
                }
            } else {
                fprintf(stderr, "Left argument must be 1 byte long. Got %zu bytes\n", left->length);
            }
            Argtype_Free(right);
        }
        Argtype_Free(left);
    }
    return ret;
}

int sbx_cli_cmd(int argc, char* argv[]) {
    if (argc != 3) {
        if (argc > 3) {
            fputs("Too many arguments, xor requires only 2 arguments", stderr);
        } else {
            puts("Usage:");
            puts("  sbx key data");
            puts("  sbx hex:05 'hello world'");
            puts("");
            puts("Arguments:");
            puts("  key     1 byte key to xor with the message");
            puts("  data    Each byte in data will be xor'd with the key byte");
            puts("");
            puts("A single byte xor is not secure. There are only");
            puts("255 possible ciphertexts for a given plaintext.");
            puts("If you believe you have a single byte xor cipher,");
            puts("you can use the cracksbx utility to decode the message.");
            puts("");
            puts("When using sbx, the first argument is treated as a char");
            puts("unless hex: or base64: is prefixed For example");
            puts("`xor sbx 5 Hello` will use 53 ('5') as the xor key.");
            puts("To use the number 5, use `hex:05` instead");
        }
        return 1;
    } else {
        return perform_sbx(argv[1], argv[2]);
    }
}

int XorCmd(int argc, char* argv[]) {
    if (argc > 1) {
        int result = Subcommand_Run(argv[1], xor_commands, argc - 1, &argv[1]);
        if (result != SUBCOMMAND_NOT_FOUND) {
            return result;
        } else {
            if (argc == 3) {
                return perform_xor(argv[1], argv[2]);
            } else if (argc > 3) {
                fputs("Too many arguments, xor requires only 2 arguments", stderr);
            } else {
                xor_usage();
            }
            return EXIT_FAILURE;
        }
    } else {
        xor_usage();
        return EXIT_FAILURE;
    }
}

int sbx_cli_crack(int argc, char* argv[]) {
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
        puts("  cracksbx [-v] ciphertext");
        puts("");
        puts("Options:");
        puts("  -v    Create a file for every key in the current directory");
        puts("");
        puts("cracksbx will crack a single byte xor cipher.");
        puts("There are only 255 possible ciphertexts for a message");
        puts("that has been 'encrypted' with this method.");
        puts("cracksbx will brute force the cipher and perform");
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
                struct XorData pt = SBX_Perform(keys.keys[i], buf.data, buf.length);
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
