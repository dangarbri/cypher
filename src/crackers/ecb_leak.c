#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "analyzers/ecb.h"
#include "cli/argtype.h"
#include "cli/system.h"
#include "operations/xor.h"
#include "parsers/base64.h"
#include "strings/replace.h"
#include "ecb_leak.h"

struct BlockDetails {
    size_t offset;
    size_t blocksize;
};

struct BlockDetails ECBLeak_FindDetails(char* command, bool base64);
struct Buffer* ECBLeak_Fuzz(char* command, char* data, bool base64);
struct BlockDetails ECBLeak_ScanXorResult(struct Buffer* xord);

const char* ECBLEAK_DELIM = "FUZZ";
const char* ECBLEAK_PREFIX = "ECBLeak";

struct Buffer* ECBLeak(char* command, bool base64, bool verbose) {
    if (strstr(command, ECBLEAK_DELIM) == NULL) {
        fputs("Missing FUZZ in command string\n", stderr);
        return NULL;
    }
    struct BlockDetails details = ECBLeak_FindDetails(command, base64);
    if ((details.offset == SIZE_MAX) || (details.blocksize == 0)) {
        fputs("Failed to determine ECB blocksize, are you sure this program is returning ECB ciphertext?\n", stderr);
    }

    if (verbose) {
        printf("Determined block size to be %zu at offset %zu\n", details.blocksize, details.offset);
    }
    return NULL;
}

/**
 * Fuzzes the command with several inputs to find the block that changes
 * The block that changes indicates the block offset
 * The idea is with ecb mode, only the block we modify will change.
 * So when we provide 2 different inputs, most of the ciphertext should be the same
 * except for the block we changed.
 * Because of this, when we xor the two blocks, we should get a lot of zeros
 * along with one block which will tell us about the blocksize, and the offset of our input.
 */
struct BlockDetails ECBLeak_FindDetails(char* command, bool base64) {
    struct BlockDetails out = {
        .offset = 0,
        .blocksize = 0
    };
    struct Buffer* left = ECBLeak_Fuzz(command, "a", base64);
    struct Buffer* middle = ECBLeak_Fuzz(command, "b", base64);
    struct Buffer* right = ECBLeak_Fuzz(command, "c", base64);
    if (left && right && middle) {
        struct Buffer* xord = Buffer_New(left->length);
        if (xord) {
            if ((left->length == right->length) && (right->length == middle->length)) {
                if (xor(xord, left, middle) == XOR_SUCCESS) {
                    struct BlockDetails take1 = ECBLeak_ScanXorResult(xord);
                    if (xor(xord, left, right) == XOR_SUCCESS) {
                        struct BlockDetails take2 = ECBLeak_ScanXorResult(xord);
                        if ((take1.blocksize == take2.blocksize) && (take1.offset == take2.offset)) {
                            out = take1;
                        }
                    }
                }

            } else {
                fprintf(stderr, "A/B test resulted in different buffer sizes %zu, %zu, %zu\n", left->length, middle->length, right->length);
            }
            Buffer_Free(xord);
        }
    }
    Buffer_Free(left);
    Buffer_Free(middle);
    Buffer_Free(right);
    return out;
}

/**
 * Scans the xor'd ciphertext result and finds the block which stands out
 */
struct BlockDetails ECBLeak_ScanXorResult(struct Buffer* xord) {
    struct BlockDetails out = {
        .offset = SIZE_MAX,
        .blocksize = 0
    };
    size_t zero_count = 0;
    for (size_t i = 0; i < xord->length; i++) {
        if ((out.offset == SIZE_MAX) && (xord->data[i] != 0)) {
            out.offset = i;
            zero_count = 0;
        }
        if ((out.offset != SIZE_MAX) && (xord->data[i] == 0)) {
            zero_count += 1;
            // 3 zeros in a row is suspicious since if we believe that ECB encryption produces 'random' output
            // then the change of 3 side by side zeros is .00000006030862941101
            // This means we've probably exited the ciphertext block
            if (zero_count > 3) {
                out.blocksize = ((i - 3) - out.offset);
                break;
            }
        }
    }
    return out;
}

/**
 * Replaces FUZZ in the command with the given data and returns the result
 */
struct Buffer* ECBLeak_Fuzz(char* command, char* data, bool base64) {
    struct Buffer* out = NULL;
    char* cmd = streplace(command, ECBLEAK_DELIM, data);
    if (cmd) {
        out = System(cmd);
        if (out && base64) {
            struct Buffer* decoded = Base64_Decode((char*) out->data);
            Buffer_Free(out);
            if (decoded) {
                out = decoded;
            } else {
                out = NULL;
            }
        }
        free(cmd);
    }
    return out;
}
