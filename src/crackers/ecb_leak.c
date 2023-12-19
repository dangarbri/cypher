#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "analyzers/ecb.h"
#include "cli/argtype.h"
#include "cli/system.h"
#include "strings/replace.h"
#include "ecb_leak.h"

const char* ECBLEAK_DELIM = "FUZZ";
const char* ECBLEAK_PREFIX = "ECBLeak";

/**
 * Checks if the command is ECB encrypting with the given block size.
 */
bool ECBLeak_CheckBlockSize(size_t blocksize, char* command) {
    // blocksize * 3 bytes guarantees 2 equal blocks in the ciphertext
    // no matter where the input starts
    size_t nbytes = blocksize * 3;
    char* replacer = malloc(nbytes + 1);
    if (!replacer) {
        return NULL;
    }
    for (size_t i = 0; i < nbytes; i++) {
        replacer[i] = 'a';
    }
    replacer[nbytes] = '\0';
    bool possible_blocksize = false;
    char* new_command = streplace(command, ECBLEAK_DELIM, replacer);
    if (new_command) {
        struct Buffer* result = System(new_command);
        if (result) {
            float score = ECB_Analyzer(result, blocksize);
            if (score > 0) {
                possible_blocksize = true;
            }
            Buffer_Free(result);
        }
        free(new_command);
    }
    free(replacer);
    return possible_blocksize;
}

/**
 * Attempts to determine the encryption blocksize of the command.
 * @param command Command template
 * @returns 0 on failure
 */
size_t ECBLeak_DetermineBlockSize(char* command) {
    size_t blocksizes[] = {64, 32, 16, 8};
    for (size_t i = 0; i < 4; i++) {
        if (ECBLeak_CheckBlockSize(blocksizes[i], command)) {
            return blocksizes[i];
        }
    }
    return 0;
}

struct Buffer* ECBLeak(char* command, bool verbose) {
    if (strstr(command, ECBLEAK_DELIM) == NULL) {
        fputs("Missing FUZZ in command string\n", stderr);
        return NULL;
    }
    size_t blocksize = ECBLeak_DetermineBlockSize(command);
    if (verbose) {
        printf("Determined block size to be %zu\n", blocksize);
    }
    return NULL;
}