#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli/argtype.h"
#include "cli/flags.h"
#include "parsers/hex.h"

void PrintInput_Usage() {
    puts("Usage:\n"
         "  print [-n] <arg>\n"
         "\n"
         "Prints the input directly to stdout.\n"
         "Useful for decoding things this program understands\n"
         "Examplse:\n"
         "  print hex:68656c6c6f\n"
         "  print base64:aGVsbG8=\n"
         "\n"
         "Options:\n"
         "  -n    Don't print a newline character at the end\n"
         "  -h    Print output as a hex string");
}

int PrintInput(int argc, char* argv[]) {
    if ((argc < 2)) {
        PrintInput_Usage();
        return EXIT_SUCCESS;
    }
    int arg_index = 1;
    bool newline = true;
    bool print_as_hex = false;
    if (FlagExists("-n", argc, argv)) {
        newline = false;
        arg_index++;
    }
    if (FlagExists("-h", argc, argv)) {
        print_as_hex = true;
        arg_index++;
    }
    if (arg_index >= argc) {
        fputs("Missing argument to print!\n", stderr);
        return EXIT_FAILURE;
    }

    char* arg_to_print = argv[arg_index];

    struct Buffer* arg = Argtype_New(arg_to_print);
    if (arg != NULL) {
        if (print_as_hex) {
            char* hex = Hex_Encode(arg);
            if (hex) {
                printf("%s", hex);
                free(hex);
            } else {
                fputs("Failed to hex encode input\n", stderr);
            }
        } else {
            // Print byte by byte.
            for (size_t i = 0; i < arg->length; i++) {
                printf("%c", arg->data[i]);
            }
        }
        Argtype_Free(arg);
        if (newline) {
            puts("");
        }
    } else {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}