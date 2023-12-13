#include <stdlib.h>
#include <stdio.h>
#include "operations/xor.h"
#include "operations/rank.h"
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
void CrackSBX_LogResult(struct Buffer* result, uint8_t key);

/**
 * Evaluates the given scores and returns the top 5 as potential keys
 * @param scores pointer to array of 256 scores, one for each key
 * @return top 5 keys
 */
struct PotentialKeys CrackSBX_Evaluate(float* scores);


struct PotentialKeys CrackSBX_SingleThread(struct Buffer* buf, Analyzer analyzer, bool verbose) {
    float scores[256] = {0};
    for (int i = 0; i < 256; i++) {
        scores[i] = CrackSBX_TestKey(buf, (uint8_t) i, analyzer, verbose);
    }
    return CrackSBX_Evaluate(&scores[0]);
}

struct PotentialKeys CrackSBX(struct Buffer* buf, Analyzer analyzer, bool verbose) {
    // Single threaded implementation
    return CrackSBX_SingleThread(buf, analyzer, verbose);
}

float CrackSBX_TestKey(struct Buffer* buffer, uint8_t key, Analyzer analyzer, bool verbose) {
    float score = -1;
    struct Buffer* result = Buffer_New(buffer->length);
    if (result != NULL) {
        if (sb_xor(result, key, buffer) == XOR_SUCCESS) {
            score = analyzer(result);
            if (verbose) {
                CrackSBX_LogResult(result, key);
            }
        }
        Buffer_Free(result);
    }
    return score;
}

void CrackSBX_LogResult(struct Buffer* result, uint8_t key) {
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

struct PotentialKeys CrackSBX_Evaluate(float* scores) {
    // Find the top 5 values
    struct PotentialKeys results = {0};
    size_t top5_keys[5];
    if (RankFloat(top5_keys, 5, scores, 256) == EXIT_SUCCESS) {
        for (int i = 0; i < 5; i++) {
            uint8_t key = (uint8_t)top5_keys[i];
            float score = scores[key];
            results.keys[i] = key;
            results.scores[i] = score;
        }
    }
    return results;
}
