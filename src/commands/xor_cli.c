#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli/argtype.h"
#include "cli/subcommand.h"
#include "parsers/hex.h"
#include "operations/xor.h"
#include "operations/sbxor.h"
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
    puts("The xor command allows you to xor items on the command line");
    puts("The output is printed as a hex string");
    puts("Inputs can be prefixed with 'hex:' or 'base64': to xor binary data on the cli");
    puts("Usage:");
    puts("  xor a b");
    puts("More Commands:");
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
            puts("Performs a single byte xor operation on the given data");
            puts("This xors every byte of data with the given key");
            puts("Key must be 1 byte long. Data can be variable length");
            puts("When using sbx, the first argument is treated as a char");
            puts("unless hex: or base64: is prefixed For example");
            puts("`xor sbx 5 Hello` will use 53 ('5') as the xor key.");
            puts("To use the number 5, use `hex:05` instead");
            puts("");
            puts("Usage:");
            puts("  sbx key data");
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
    (void)argv;
    if (argc != 2) {
        puts("cracksbx will crack a single byte xor cipher.");
        puts("There are only 255 possible permutations of a single byte xor cipher");
        puts("cracksbx will analyze each permutation and perform");
        puts("frequency analysis on the results to choose the");
        puts("permutation which seems the most like english plaintext");
        puts("Currently only the english language is supported");
        puts("Pull requests to support other languages would be nice");
        puts("");
        puts("Usage:");
        puts("  cracksbx ciphertext");
        return EXIT_FAILURE;
    } else {
        puts("Not implemented yet");
    }
    return 0;
}
