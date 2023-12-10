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
        return -1;
    }
    char* message_to_encode = argv[1];
    size_t length = strlen(message_to_encode);
    struct Arg* arg = Argtype_New(message_to_encode);
    if (arg->type == ARGTYPE_BINARY) {
        message_to_encode = (char*) arg->data;
        length = arg->length;
    }
    struct Base64Data data = Base64_Encode((uint8_t*) message_to_encode, length);
    Argtype_Free(arg);
    if (!data.valid) {
        fputs("Sorry, I couldn't encode that!\n", stderr);
        return -1;
    }
    printf("%s\n", data.data);
    free(data.data);
    return 0;
}

int b64_decode(int argc, char** argv) {
    if (argc < 2) {
        fputs("Missing argument\n", stderr);
        return -1;
    }
    char* message_to_decode = argv[1];
    struct Base64Data data = Base64_Decode(message_to_decode);
    if (!data.valid) {
        fputs("Sorry, I couldn't decode that!\n", stderr);
        return -1;
    }
    printf("%s\n", data.data);
    free(data.data);
    return 0;
}

int Base64(int argc, char** argv) {
    if (argc <= 1) {
        b64_print_usage(0, NULL);
        return 1;
    } else {
        return Subcommand_Run(argv[1], b64_commands, argc - 1, &argv[1]);
    }
    return 0;
}
