#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

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

/**
 * Stores a mapping of a byte to a ciphertext block
 */
struct ByteCipherPair {
    char byte;
    uint8_t* block;
};

typedef struct ByteCipherPair* ECBCipherMap;

struct BlockDetails ECBLeak_FindDetails(char* command, size_t input_size, bool base64);
struct Buffer* ECBLeak_Fuzz(char* command, char* data, bool base64);
struct BlockDetails ECBLeak_ScanXorResult(struct Buffer* xord);
size_t ECBLeak_FindBlockCrossing(char* command, size_t blocksize, bool base64);
uint8_t ECBLeak_CrackByte(char* command, size_t target_block, size_t input_length, size_t blocksize, bool base64);
struct Buffer* ECBLeak_ExtractBlock(char* command, size_t target_block, size_t input_length, size_t details, uint8_t value, bool base64);

const char* ECBLEAK_DELIM = "FUZZ";
const char* ECBLEAK_PREFIX = "ECBLeak";

struct Buffer* ECBLeak(char* command, bool base64, bool verbose) {
    if (strstr(command, ECBLEAK_DELIM) == NULL) {
        fputs("Missing FUZZ in command string\n", stderr);
        return NULL;
    }
    struct BlockDetails details = ECBLeak_FindDetails(command, 1, base64);
    if ((details.offset == SIZE_MAX) || (details.blocksize == 0)) {
        fputs("Failed to determine ECB blocksize, are you sure this program is returning ECB ciphertext?\n", stderr);
        return NULL;
    }

    size_t block_crossing = ECBLeak_FindBlockCrossing(command, details.blocksize, base64);
    if (block_crossing < 2) {
        fputs("Failed to find block crossing. This is probably an implementation error\n", stderr);
        return NULL;
    }

    if (verbose) {
        printf("Our input cross two blocks when we insert %zu bytes\n", block_crossing);
        printf("Determined block size to be %zu at offset %zu\n", details.blocksize, details.offset);
        printf("Message: ");
    }

    // Set the input length to be large enough that we control a whole block, but leave one byte short.
    // This will make it so that one byte of the unknown plaintext will get encrypted with our
    // malicious block
    size_t input_length = block_crossing + details.blocksize - 2;
    uint8_t byte = ECBLeak_CrackByte(command, details.offset + details.blocksize, input_length, details.blocksize, base64);
    printf("%c", byte);
    puts("");

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
struct BlockDetails ECBLeak_FindDetails(char* command, size_t input_size, bool base64) {
    struct BlockDetails out = {
        .offset = SIZE_MAX,
        .blocksize = 0
    };
    char* fuzz_text = malloc(input_size + 1);
    if (!fuzz_text) {
        return out;
    }
    fuzz_text[input_size] = '\0';
    memset(fuzz_text, 'a', input_size);
    struct Buffer* left = ECBLeak_Fuzz(command, fuzz_text, base64);
    fuzz_text[0] += 1;
    fuzz_text[input_size - 1] += 1;
    struct Buffer* middle = ECBLeak_Fuzz(command, fuzz_text, base64);
    fuzz_text[0] += 1;
    fuzz_text[input_size - 1] += 1;
    struct Buffer* right = ECBLeak_Fuzz(command, fuzz_text, base64);
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
    free(fuzz_text);
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

/**
 * Finds the block crossing offset.
 * This is how many bytes we need to insert so that our input crosses
 * the block boundary.
 * @returns 0 on failure
 */
size_t ECBLeak_FindBlockCrossing(char* command, size_t blocksize, bool base64) {
    size_t upper_limit = blocksize + 1;
    for (size_t i = 2; i < upper_limit; i++) {
        struct BlockDetails details = ECBLeak_FindDetails(command, i, base64);
        if (details.blocksize > blocksize) {
            return i;
        }
    }
    return 0;
}

/**
 * Crack a single byte where input length is 1 byte shorter than the block length
 */
uint8_t ECBLeak_CrackByte(char* command, size_t target_block, size_t input_length, size_t blocksize, bool base64) {
    uint8_t result = 0;
    // Extract a ciphertext block
    struct Buffer* cipherblock = ECBLeak_ExtractBlock(command, target_block, input_length, blocksize, (uint8_t) 'a', base64);
    if (cipherblock) {
        for (size_t i = 1; i < 256; i++) {
            uint8_t value = (uint8_t) i;
            struct Buffer* testblock = ECBLeak_ExtractBlock(command, target_block, input_length + 1, blocksize, value, base64);
            if (testblock) {
                if (memcmp(testblock->data, cipherblock->data, blocksize) == 0) {
                    result = value;
                    break;
                }
                Buffer_Free(testblock);
            }
        }
        Buffer_Free(cipherblock);
    }
    return result;
}

/**
 * Returns a block of ciphertext from the command
 * @param command Command string
 * @param target_block Offset of the block to return
 * @param input_length number of bytes to send to the command
 * @param blocksize Block size
 * @param base64 flags that command returns base64 data
 */
struct Buffer* ECBLeak_ExtractBlock(char* command, size_t target_block, size_t input_length, size_t blocksize, uint8_t value, bool base64) {
    struct Buffer* out = NULL;
    // Allocate space for fuzz text
    char* fuzz = malloc(input_length + 1);
    if (fuzz) {
        fuzz[input_length] = '\0';
        memset(fuzz, 'a', input_length);
        fuzz[input_length - 1] = value;
        struct Buffer* cipher = ECBLeak_Fuzz(command, fuzz, base64);
        if (cipher) {
            size_t end = target_block + blocksize;
            if (cipher->length >= end) {
                out = Buffer_New(blocksize + 1);
                if (out) {
                    out->data[blocksize] = '\0';
                    memcpy(out->data, &cipher->data[target_block], blocksize);
                    Buffer_Free(cipher);
                }
            } else {
                fprintf(stderr, "Invalid target block. Requested %zu, ciphertext length: %zu\n", end, cipher->length);
            }
        }
        free(fuzz);
    }
    return out;
}
