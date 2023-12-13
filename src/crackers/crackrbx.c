#include <stdio.h>
#include <stdlib.h>

#include "operations/hamming.h"
#include "operations/rank.h"
#include "cracksbx.h"
#include "crackrbx.h"

#define NUM_KEYS_TO_TEST (CRACKRBX_MAX_KEYS)

struct RepeatingXorKeys* CrackRepeatingXor(size_t keymin, size_t keymax, struct Buffer* buf, Analyzer analyzer, bool verbose) {
    if (verbose) {
        puts("Analyzing ciphertext for the best key size...");
    }

    struct RankedKeySize rank;
    int result = CrackRBX_RankKeySizes(&rank, keymin, keymax, buf);
    if (result == EXIT_FAILURE) {
        return NULL;
    }

    // Count the number
    size_t num_keys = (keymax - keymin) + 1;
    size_t num_keys_to_test = num_keys < NUM_KEYS_TO_TEST ? num_keys : NUM_KEYS_TO_TEST;

    struct RepeatingXorKeys* keylist = malloc(sizeof(struct RepeatingXorKeys));
    if (keylist != NULL) {
        keylist->keys = calloc(num_keys_to_test, sizeof(keylist->keys));
        keylist->length = num_keys_to_test;
        if (keylist->keys != NULL) {
            for (size_t i = 0; i < num_keys_to_test; i++) {
                keylist->keys[i] = CrackRepeatingXor_WithKeyLength(rank.keysize[i], buf, analyzer, verbose);
                if (keylist->keys[i] == NULL) {
                    fprintf(stderr, "Error occurred while cracking repeating xor with key length %zu\n", rank.keysize[i]);
                    CrackRBX_FreeKeys(keylist);
                    return NULL;
                }
            }
            // Success hidden in the mess...
            if (verbose) {
                puts("We may have cracked it!");
            }
            return keylist;
        } else {
            perror("CrackRBX");
        }
        CrackRBX_FreeKeys(keylist);
    } else {
        perror("CrackSBX");
    }
    return NULL;
}

int CrackRBX_RankKeySizes(struct RankedKeySize* out, size_t keymin, size_t keymax, struct Buffer* buf) {
    // Ciphertext needs to be at least double the keysize to even attempt this crack.
    // If the length of the buffer is less than 2 minimum key chunks, then we can't proceed.
    if ((buf->length < (keymin * 2)) || (keymin == 0) || (keymin > keymax)) {
        return EXIT_FAILURE;
    }
    // allocate a score array big enough to hold all the rankings
    size_t num_keys = (keymax - keymin) + 1;
    float scores[num_keys];
    for (size_t keysize = keymin; keysize <= keymax; keysize++) {
        // For each KEYSIZE, take the first KEYSIZE worth of bytes,
        // and the second KEYSIZE worth of bytes, and find the edit distance between them.
        // Normalize this result by dividing by KEYSIZE.
        // The KEYSIZE with the smallest normalized edit distance is probably the key.
        size_t num_chunks = buf->length / keysize;
        if (num_chunks == 1) {
            scores[keysize - keymin] = 1000;
            continue;
        }
        size_t total_hamming_distance = 0;
        for (size_t i = 0; i < (num_chunks - 1); i++) {
            struct Buffer left_chunk = {
                .data = (uint8_t*) &buf->data[i * keysize],
                .length = keysize
            };
            struct Buffer right_chunk = {
                .data = (uint8_t*) &buf->data[(i+1) * keysize],
                .length = keysize
            };
            size_t result = GetHammingDistance(&left_chunk, &right_chunk);
            if (result == SIZE_MAX) {
                return EXIT_FAILURE;
            }
            total_hamming_distance += result;
        }
        float average_hamming_distance = ((float) total_hamming_distance) / (num_chunks - 1);
        float normalized_rank = average_hamming_distance / keysize;
        scores[keysize - keymin] = normalized_rank;
    }

    size_t ranking[num_keys];
    if (RankFloat(ranking, num_keys, scores, num_keys) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    size_t min = num_keys < NUM_KEYS_TO_TEST ? num_keys : NUM_KEYS_TO_TEST;
    for (size_t i = 0; i < min; i++) {
        out->keysize[i] = ranking[keymax - keymin - i] + keymin;
    }
    return EXIT_SUCCESS;
}

struct Buffer* CrackRepeatingXor_WithKeyLength(size_t keylength, struct Buffer* buf, Analyzer analyzer, bool verbose) {
    (void)analyzer;
    (void)verbose;
    if ((keylength == 0) || (buf == NULL)) {
        return NULL;
    }

    struct Buffer* key = Buffer_New(keylength);
    if (key == NULL) {
        return NULL;
    }

    return key;
}

void CrackRBX_FreeKeys(struct RepeatingXorKeys* keys) {
    if (keys != NULL) {
        if (keys->keys != NULL) {
            for (size_t i = 0; i < keys->length; i++) {
                if (keys->keys[i] != NULL) {
                    Buffer_Free(keys->keys[i]);
                }
            }
            free(keys->keys);
        }
        free(keys);
    }
}
