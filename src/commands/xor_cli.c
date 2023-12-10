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
int perform_xor(char* a, char* b);

struct Subcommand xor_commands[] = {
    {"sbx", "single byte xor operation", sbx_cli_cmd},
    {NULL}
};

void xor_usage() {
    puts("The xor command allows you to xor items on the command line");
    puts("The output is printed as a hex string");
    puts("Inputs can be prefixed with 'hex:' or 'base64': to xor binary data on the cli");
    puts("When using sbx, the first argument is treated as a char unless hex: or base64: is prefixed");
    puts("For example `xor sbx 5 Hello` will use 53 ('5') as the xor key. To use the number 5, use `hex:05`");
    puts("Usage:");
    puts("  xor a b");
    puts("More Commands:");
    Subcommand_PrintHelp(xor_commands);
}

int perform_xor(char* a, char* b) {
    int ret = -1;
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
                        ret = 0;
                        free(encoded);
                    } else {
                        fputs("Failed to hex encode xor result", stderr);
                    }
                    free(result);
                } else {
                    perror("xor_cli: ");
                }
            } else {
                fprintf(stderr, "Left and right arguments are not the same length (%ld != %ld)\n", left->length, right->length);
            }
            Argtype_Free(right);
        }
        Argtype_Free(left);
    }
    return ret;
}

int perform_sbx(char* a, char* b) {
    int ret = -1;
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
                        ret = 0;
                        free(encoded);
                    }
                    free(result.data);
                }
            } else {
                fprintf(stderr, "Left argument must be 1 byte long. Got %ld bytes\n", left->length);
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
            xor_usage();
        }
        return -1;
    } else {
        return perform_sbx(argv[1], argv[2]);
    }
    return 0;
}

int XorCmd(int argc, char* argv[]) {
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
        return -1;
    }
}
