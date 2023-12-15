#include <stdio.h>
#include <stdlib.h>

#include "operations/hamming.h"
#include "operations/rank.h"
#include "parsers/hex.h"
#include "threading/pool.h"

#include "cracksbx.h"
#include "crackrbx.h"

#define NUM_KEYS_TO_TEST (CRACKRBX_MAX_KEYS)
struct CrackRbxThreadArgs {
    size_t keylength;
    const struct Buffer* buf;
    Analyzer analyzer;
    bool verbose;
    struct Buffer** out;
};

/**
 * Executes CrackRepeatingXor_WithKeyLength within a thread and writes the result to arg
 * (benchmarking shows the threaded version runs in ~40ms while the single threaded version runs in ~100ms)
 */
int CrackRepeatingXor_WithKeyLength_Thread(void* arg);

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
    if (verbose) {
        printf("Choosing to test key lengths: ");
        for (size_t i = 0; i < num_keys_to_test; i++) {
            printf("%zu ", rank.keysize[i]);
        }
        puts("");
        puts("");
    }

    struct RepeatingXorKeys* keylist = NULL;
    struct ThreadPool* pool = ThreadPool_New(5);
    if (pool != NULL) {
        struct ThreadQueue* queue = ThreadQueue_New(num_keys_to_test);
        struct CrackRbxThreadArgs* args = malloc(num_keys_to_test * sizeof(struct CrackRbxThreadArgs));
        if (args != NULL) {
            if (queue != NULL) {
                keylist = malloc(sizeof(struct RepeatingXorKeys));
                if (keylist != NULL) {
                    keylist->keys = calloc(num_keys_to_test, sizeof(keylist->keys));
                    bool failure = true;
                    if (keylist->keys != NULL) {
                        keylist->length = num_keys_to_test;
                        for (size_t i = 0; i < num_keys_to_test; i++) {
                            args[i].analyzer = analyzer;
                            args[i].buf = buf;
                            args[i].keylength = rank.keysize[i];
                            args[i].verbose = verbose;
                            args[i].out = &keylist->keys[i];
                            queue->info[i].Function = CrackRepeatingXor_WithKeyLength_Thread;
                            queue->info[i].arg = &args[i];
                        }
                        thrd_t bg_thread = ThreadPool_SubmitQueue(pool, queue);
                        if (ThrdGood(bg_thread)) {
                            if ((thrd_join(bg_thread, &result) == thrd_success) && result != -1) {
                                // Success finally in the mess...
                                failure = false;
                                if (verbose) {
                                    CrackRBX_PrintSummary(keylist);
                                }
                            }
                        }
                        else {
                            fputs("Failed to start crackrbx threads\n", stderr);
                        }
                    }
                    if (failure) {
                        CrackRBX_FreeKeys(keylist);
                        keylist = NULL;
                        fputs("CrackRBX failed\n", stderr);
                    }
                }
                else {
                    perror("CrackSBX");
                }
                ThreadQueue_Free(queue);
            }
            free(args);
        }
        ThreadPool_Free(pool);
    }
    return keylist;
}

int CrackRBX_RankKeySizes(struct RankedKeySize* out, size_t keymin, size_t keymax, struct Buffer* buf) {
    // Ciphertext needs to be at least double the keysize to even attempt this crack.
    // If the length of the buffer is less than 2 minimum key chunks, then we can't proceed.
    if ((buf->length < (keymin * 2)) || (keymin == 0) || (keymin > keymax)) {
        return EXIT_FAILURE;
    }
    // allocate a score array big enough to hold all the rankings
    size_t num_keys = (keymax - keymin) + 1;
    float* scores = malloc(num_keys * sizeof(float));
    if (scores != NULL) {
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
                    .data = (uint8_t*)&buf->data[i * keysize],
                    .length = keysize
                };
                struct Buffer right_chunk = {
                    .data = (uint8_t*)&buf->data[(i + 1) * keysize],
                    .length = keysize
                };
                size_t result = GetHammingDistance(&left_chunk, &right_chunk);
                if (result == SIZE_MAX) {
                    free(scores);
                    return EXIT_FAILURE;
                }
                total_hamming_distance += result;
            }
            float average_hamming_distance = ((float)total_hamming_distance) / (num_chunks - 1);
            float normalized_rank = average_hamming_distance / keysize;
            scores[keysize - keymin] = normalized_rank;
        }

        size_t* ranking = malloc(num_keys * sizeof(size_t));
        if (ranking != NULL) {
            if (RankFloat(ranking, num_keys, scores, num_keys) == EXIT_FAILURE) {
                return EXIT_FAILURE;
            }

            size_t min = num_keys < NUM_KEYS_TO_TEST ? num_keys : NUM_KEYS_TO_TEST;
            for (size_t i = 0; i < min; i++) {
                out->keysize[i] = ranking[keymax - keymin - i] + keymin;
            }
            free(ranking);
        }
        free(scores);
    }    
    return EXIT_SUCCESS;
}

struct Buffer* CrackRepeatingXor_WithKeyLength(size_t keylength, const struct Buffer* buf, Analyzer analyzer, bool verbose) {
    if ((keylength == 0) || (buf == NULL)) {
        return NULL;
    }
    FILE* verbose_fp = NULL;

    if (verbose) {
        verbose_fp = tmpfile();
        if (verbose_fp == NULL) {
            verbose_fp = stdout;
            fputs("Failed to allocate tmpfile for de-interleaving thread output. Output will likely be scrambled", stderr);
        }
        fprintf(verbose_fp, "Testing key length %zu\n", keylength);
        fputs("\n", verbose_fp);
    }

    /**
     * Now that you probably know the KEYSIZE: break the ciphertext into blocks of KEYSIZE length.
     * Now transpose the blocks: make a block that is the first byte of every block, and a block that is the second byte of every block, and so on.
     */
    struct Buffer* key = Buffer_New(keylength);
    if (key != NULL) {
        struct Buffer* transposed = Buffer_Transpose(buf, keylength);
        if (transposed != NULL) {
            size_t num_blocks = buf->length / keylength;
            // After transposing, we have strings of letters that were xor'd with the same byte
            // that we can attempt to crack with the single byte xor method
            for (size_t i = 0; i < keylength; i++) {
                struct Buffer cipher = {
                    .data = &transposed->data[i * num_blocks],
                    .length = num_blocks
                };
                struct PotentialKeys cracked_keys = CrackSBX(&cipher, analyzer, false);
                if (verbose) {
                    fprintf(verbose_fp, "Byte %zu: ", i);
                    CrackSBX_PrintKeys(&cracked_keys, verbose_fp);
                    fputs("\n", verbose_fp);
                }
                key->data[i] = cracked_keys.keys[0];
            }
            if (verbose) {
                char* keystr = Hex_Encode(key);
                if (keystr != NULL) {
                    fprintf(verbose_fp, "Possible key -> hex:%s\n", keystr);
                    free(keystr);
                }
            }
            Buffer_Free(transposed);
        } else {
            Buffer_Free(key);
            key = NULL;
        }
    }

    if (verbose) {
        char null_terminator = '\0';
        fwrite(&null_terminator, 1, 1, verbose_fp);
        long size = ftell(verbose_fp);
        char* textbuf = malloc((size_t) size);
        if (textbuf != NULL) {
            rewind(verbose_fp);
            size_t bytes = fread(textbuf, 1, size, verbose_fp);
            if (bytes > 0) {
                Thread_printf("%s\n", textbuf);
                fclose(verbose_fp);
            } else {
                fputs("Error streaming log buffer to stdout\n", stderr);
            }
            free(textbuf);
        } else {
            fputs("Couldn't allocate memory, logs are gone\n", stderr);
        }
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

void CrackRBX_PrintSummary(struct RepeatingXorKeys* keylist) {
    if (keylist != NULL) {
        printf("Found %zu potential keys\n", keylist->length);
        for (size_t i = 0; i < keylist->length; i++) {
            char* keystr = Hex_Encode(keylist->keys[i]);
            if (keystr != NULL) {
                printf("%zu: %4zu Bytes -> hex:%s\n", i, keylist->keys[i]->length, keystr);
                free(keystr);
            }
        }
    }
}

int CrackRepeatingXor_WithKeyLength_Thread(void* arg) {
    struct CrackRbxThreadArgs* args = (struct CrackRbxThreadArgs*) arg;
    *args->out = CrackRepeatingXor_WithKeyLength(args->keylength, args->buf, args->analyzer, args->verbose);
    return 0;
}
