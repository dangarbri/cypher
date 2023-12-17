#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <openssl/rand.h>

#include "cli/flags.h"
#include "rand.h"

void RandCli_Usage() {
    puts(
        "Usage:\n"
        "  rand [-b] <count> # Generate random bytes\n"
        "\n"
        "rand generates random bytes using openssl's rand interface.\n"
        "By default, data is hex encoded and printed to stdout.\n"
        "To print raw bytes, use the -b option.\n"
        "\n"
        "Options:\n"
        "  -b    Print raw bytes instead of hex encoding"
    );
}

int RandCli(int argc, char* argv[]) {
    if (argc < 2) {
        RandCli_Usage();
        return EXIT_FAILURE;
    }

    int count_index = 1;
    bool hex = true;
    if (FlagExists("-b", argc, argv)) {
        hex = false;
        count_index++;
    }
    if (count_index >= argc) {
        fputs("Missing requested byte count\n", stderr);
        return EXIT_FAILURE;
    }

    char* count_str = argv[count_index];
    char* invalid = NULL;
    errno = 0;
    unsigned long long count = strtoull(count_str, &invalid, 10);
    if (errno) {
        fputs("Invalid characters found at %s: ", stderr);
        perror(NULL);
        return EXIT_FAILURE;
    }

    if (!hex) {
        // make stdout unbuffered so we can write bytes freely.
        setvbuf(stdout, NULL, _IONBF, 0);
    }

    uint8_t rand_byte;
    for (unsigned long long i = 0; i < count; i++) {
        if (RAND_bytes(&rand_byte, 1)) {
            if (hex) {
                printf("%02x", rand_byte);
            } else {
                fwrite(&rand_byte, 1, 1, stdout);
            }
        } else {
            fputs("Couldn't generate randomness\n", stderr);
            return EXIT_FAILURE;
        }
    }

    if (hex) {
        puts("");
    }

    return EXIT_SUCCESS;
}