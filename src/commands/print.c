#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli/argtype.h"

void PrintInput_Usage() {
    puts("Usage:\n"
         "  print [-n] <arg>\n"
         "\n"
         "Prints the inputs directly to stdout.\n"
         "Useful for printing a hex string as ascii.\n"
         "Supports hex: and base64: prefixed inputs.\n"
         "\n"
         "Options:\n"
         "  -n    Don't print a newline character at the end");
}

int PrintInput(int argc, char* argv[]) {
    if ((argc < 2) || (argc > 3)) {
        PrintInput_Usage();
        return EXIT_SUCCESS;
    }
    int arg_index = 1;
    bool newline = true;
    if (strncmp(argv[1], "-n", 2) == 0) {
        if (argc != 3) {
            puts("Missing argument");
            return EXIT_FAILURE;
        }
        arg_index = 2;
        newline = false;
    } else {
        if (argc != 2) {
            puts("Too many args");
            return EXIT_FAILURE;
        }
    }
    struct Arg* arg = Argtype_New(argv[arg_index]);
    if (arg != NULL) {
        for (size_t i = 0; i < arg->buffer.length; i++) {
            printf("%c", arg->buffer.data[i]);
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