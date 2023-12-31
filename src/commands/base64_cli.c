#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "base64_cli.h"
#include "cli/subcommand.h"
#include "cli/argtype.h"
#include "parsers/base64.h"

int b64_print_usage(int argc, char* argv[]);
int b64_print_help(int argc, char* argv[]);
int b64_decode(int argc, char* argv[]);
int b64_encode(int argc, char* argv[]);

struct Subcommand b64_commands[] = {
    {"-h", "Show detailed help message", b64_print_help},
    {"-d", "Decode an encoded string", b64_decode},
    {"-e", "Encode some data", b64_encode},
    {NULL}
};

int b64_print_usage(int argc, char* argv[]) {
    // supress warning
    (void) argc;
    (void) argv;
    puts("This module implements base64 encoding and decoding utilities");
    puts("");
    puts("OPTIONS:");
    Subcommand_PrintHelp(b64_commands);
    return 0;
}

int b64_print_help(int argc, char** argv) {
    puts("For encoding, you may use the hex: modifier to send binary data as a hex string");
    puts("EXAMPLE:");
    puts("  This would encode the bytes de ad be ef. Not the word deadbeef");
    puts("  base64 -e hex:deadbeef");
    puts("");
    puts("OPTIONS:");
    puts("  -d encoded_message    Decodes encoded_messages and writes result to stdout");
    puts("  -e message            Encodes message and writes result to stdout");
    (void) argc;
    (void) argv;
    return 0;
}

int b64_encode(int argc, char** argv) {
    if (argc < 2) {
        fputs("Missing argument\n", stderr);
        return EXIT_FAILURE;
    }
    struct Buffer* arg = Argtype_New(argv[1]);
    if (arg == NULL) {
        return EXIT_FAILURE;
    }
    struct Buffer* data = Base64_Encode((uint8_t*) arg->data, arg->length);
    Argtype_Free(arg);
    if (data == NULL) {
        fputs("Sorry, I couldn't encode that!\n", stderr);
        return EXIT_FAILURE;
    }
    printf("%s\n", data->data);
    Buffer_Free(data);
    return 0;
}

int b64_decode(int argc, char** argv) {
    if (argc < 2) {
        fputs("Missing argument\n", stderr);
        return EXIT_FAILURE;
    }
    struct Buffer* arg = Argtype_New(argv[1]);
    if (arg == NULL) {
        return EXIT_FAILURE;
    }
    struct Buffer* data = Base64_Decode((char*) arg->data);
    Argtype_Free(arg);
    if (data == NULL) {
        fputs("Sorry, I couldn't decode that!\n", stderr);
        return EXIT_FAILURE;
    }
    printf("%s\n", data->data);
    Buffer_Free(data);
    return 0;
}

int Base64(int argc, char** argv) {
    if (argc <= 1) {
        b64_print_usage(0, NULL);
        return 1;
    } else {
        int result = Subcommand_Run(argv[1], b64_commands, argc - 1, &argv[1]);
        if (result == SUBCOMMAND_NOT_FOUND) {
            fprintf(stderr, "Invalid option '%s'\n", argv[1]);
        }
        return result;
    }
}
