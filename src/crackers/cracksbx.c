#include <stdlib.h>
#include <stdio.h>
#include <threads.h>

#include "operations/sbxor.h"
#include "cracksbx.h"

/**
 * Tests the likelihood that key is the right one for this buffer.
 * @param key Key to test
 * @param buffer Buffer to examine
 * @param analyzer Analyzer used to score the results
 * @param verbose Logs results to an output file when logged
 * @return score or a value < 0 on failure.
 */
float CrackSBX_TestKey(struct Buffer* buffer, uint8_t key, Analyzer analyzer, bool verbose);

/**
 * Logs the given data to a file identified by the given key
 * @param result Xor data to write to the file
 * @param key Key that produced this result
 */
void CrackSBX_LogResult(struct XorData* result, uint8_t key);

#ifdef __STDC_NO_THREADS__
#else

struct ThreadArgs {
    struct Buffer* data;
    uint8_t key_to_test;
    Analyzer analyzer;
    bool verbose;
};

int CrackSBX_Thread(void* data) {
    struct ThreadArgs* args = (struct ThreadArgs*) data;
    float result = CrackSBX_TestKey(args->data, args->key_to_test, args->analyzer, args->verbose);
    return (int) (result * 1000);
}

struct PotentialKeys CrackSBX_Multithreaded(struct Buffer* buf, Analyzer analyzer, bool verbose) {
    struct PotentialKeys keys = {0};
    // Create threads to test keys from 0 to 255
    thrd_t threads[256] = {0};
    int scores[256] = {0};
    struct ThreadArgs args[256];
    // Spin up a thread for each possible byte to test
    for (int i = 0; i < 256; i++) {
        args[i].analyzer = analyzer;
        args[i].data = buf;
        args[i].key_to_test = (uint8_t) i;
        args[i].verbose = verbose;
        if (thrd_create(&threads[i], CrackSBX_Thread, &args[i]) != thrd_success) {
            fputs("Failed to start thread for cracksbx", stderr);
            break;
        }
    }
    // Wait for all the threads to complete, and store their results in the scores array
    for (int i = 0; i < 256; i++) {
        // The thread may not be initialized if thrd_create failed.
        if (threads[i] != 0) {
            if (thrd_join(threads[i], &scores[i]) == thrd_error) {
                fputs("Failed to join cracksbx thread, but going to keep running anyway", stderr);
            }
        }
    }
    // Find the top 5 values
    uint8_t max_bytes[5] = {0};
    int max_counts[5] = {0};
    // Find the top 12 characters
    for (int i = 0; i < 256; i++) {
        uint8_t byte = (uint8_t) i;
        int count = scores[i];
        for (int j = 0; j < 5; j++) {
            bool is_larger = count > max_counts[j];
            // index 0 has the biggest max, set it unconditionally
            if (((j == 0) && is_larger) ||
                (is_larger && (count <= max_counts[j-1]) && (byte != max_bytes[j-1]))) {
                    // Shift the array to make space for this new max value
                    for(int k = 4; k > j; k--) {
                        max_counts[k] = max_counts[k-1];
                        max_bytes[k] = max_bytes[k-1];
                    }
                max_counts[j] = count;
                max_bytes[j] = byte;
            }
        }
    }

    for (int i = 0; i < 5; i++) {
        keys.keys[i] = max_bytes[i];
        keys.scores[i] = (float) max_counts[i];
    }
    return keys;
}
#endif

struct PotentialKeys CrackSBX(struct Buffer* buf, Analyzer analyzer, bool verbose) {
    #ifdef __STDC_NO_THREADS__
    // Single threaded implementation
    struct PotentialKeys keys;
    return keys;
    #else
    // Multithreaded implementation
    // I know there's only 255 possibilities so a single threaded solution be fast enough,
    // but I'm interested in learning the C11 threading library, and this is an
    // easy enough place to try it out.
    return CrackSBX_Multithreaded(buf, analyzer, verbose);
    #endif
}

float CrackSBX_TestKey(struct Buffer* buffer, uint8_t key, Analyzer analyzer, bool verbose) {
    float score = -1;
    struct XorData result = SBX_Perform(key, buffer->data, buffer->length);
    if (result.data != NULL) {
        struct Buffer result_buffer = {
            .data = result.data,
            .length = result.length
        };
        score = analyzer(&result_buffer);
        if (verbose) {
            CrackSBX_LogResult(&result, key);
        }
        free(result.data);
    }
    return score;
}

void CrackSBX_LogResult(struct XorData* result, uint8_t key) {
    // Create the file to write to
    char* format = "cracksbx_%02X.txt";
    char fname[16];
    snprintf(fname, 16, format, key);
    FILE* fp = fopen(fname, "w");
    if (fp != NULL) {
        // Write data to the file
        fwrite(result->data, 1, result->length, fp);
        // Close the file
        fclose(fp);
    } else {
        perror("Can't open file for logging CrackSBX results");
    }
}

