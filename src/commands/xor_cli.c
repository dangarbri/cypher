#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli/argtype.h"
#include "cli/subcommand.h"
#include "parsers/hex.h"
#include "operations/xor.h"
#include "xor_cli.h"

void xor_usage() {
    puts("The xor command allows you to xor items on the command line");
    puts("The output is printed as a hex string");
    puts("Inputs can be prefixed with 'hex:' or 'base64': to xor binary data on the cli");
    puts("Usage:");
    puts("  xor a b");
}

int perform_xor(char* a, char* b) {
    int result = -1;
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
    return result;
}


int XorCmd(int argc, char* argv[]) {
    if (argc != 3) {
        if (argc > 3) {
            fputs("Too many arguments, xor requires only 2 arguments", stderr);
        } else {
            xor_usage();
        }
        return -1;
    } else {
        return perform_xor(argv[1], argv[2]);
    }
}
