#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "analyzers/english.h"
#include "cli/argtype.h"
#include "cli/subcommand.h"
#include "operations/xor.h"
#include "parsers/hex.h"
#include "types/buffer.h"
#include "xor_cli.h"

int sbx_cli_cmd(int argc, char* argv[]);
int repeating_xor_cmd(int argc, char* argv[]);
int perform_xor(char* a, char* b);

struct Subcommand xor_commands[] = {
    {"sbx", "single byte xor operation", sbx_cli_cmd},
    {"rbx", "repeating byte xor operation", repeating_xor_cmd},
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
    struct Buffer* left = Argtype_New(a);
    if (left != NULL) {
        struct Buffer* right = Argtype_New(b);
        if (right != NULL) {
            if (left->length == right->length) {
                struct Buffer* result = Buffer_New(right->length);
                if (result != NULL) {
                    enum XorResult xor_result = xor(result, left, right);
                    if (xor_result == XOR_SUCCESS) {
                        char* encoded = Hex_Encode(result);
                        if (encoded != NULL) {
                            printf("%s\n", encoded);
                            ret = EXIT_SUCCESS;
                            free(encoded);
                        } else {
                            fputs("Failed to hex encode xor result", stderr);
                        }
                    }
                    Buffer_Free(result);
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
    struct Buffer* left = Argtype_New(a);
    if (left != NULL) {
        struct Buffer* right = Argtype_New(b);
        if (right != NULL) {
            if (left->length == 1) {
                struct Buffer* result = Buffer_New(right->length);
                if (result != NULL) {
                    if (sb_xor(result, *left->data, right) == XOR_SUCCESS) {
                        char* encoded = Hex_Encode(result);
                        if (encoded != NULL) {
                            printf("%s\n", encoded);
                            ret = EXIT_SUCCESS;
                            free(encoded);
                        }
                    }
                    Buffer_Free(result);
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
            puts("you can use the crack utility to decode the message.");
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

int repeating_xor_cmd(int argc, char* argv[]) {
    if (argc != 3) {
        puts("Usage:\n"
             "  rbx key message\n"
             "\n"
             "Repeating byte xor will sequentially xor each byte of key\n"
             "with each byte of message. When the last byte of key is\n"
             "reached, then the operation will continue from the first\n"
             "byte of key. The result is that message is basically obfuscated\n"
             "in chunks which are the size of the key length.");
        return EXIT_FAILURE;
    }
    int ret = EXIT_FAILURE;
    struct Buffer* key = Argtype_New(argv[1]);
    if (key != NULL) {
        struct Buffer* message = Argtype_New(argv[2]);
        if (message != NULL) {
            struct Buffer* result = Buffer_New(message->length);
            if (result != NULL) {
                int status = rp_xor(result, key, message);
                if (status == XOR_SUCCESS) {
                    char* encoded = Hex_Encode(result);
                    if (encoded != NULL) {
                        printf("%s\n", encoded);
                        ret = EXIT_SUCCESS;
                        free(encoded);
                    }
                }
                Buffer_Free(result);
            }
            Argtype_Free(message);
        }
        Argtype_Free(key);
    }
    return ret;
}
