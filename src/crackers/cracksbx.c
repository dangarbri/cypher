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

/**
 * Evaluates the given scores and returns the top 5 as potential keys
 * @param scores pointer to array of 256 scores, one for each key
 * @return top 5 keys
 */
struct PotentialKeys CrackSBX_Evaluate(int* scores);


struct PotentialKeys CrackSBX_SingleThread(struct Buffer* buf, Analyzer analyzer, bool verbose) {
    int scores[256] = {0};
    for (int i = 0; i < 256; i++) {
        float result = CrackSBX_TestKey(buf, (uint8_t) i, analyzer, verbose);
        scores[i] = (int) (result * 1000);
    }
    return CrackSBX_Evaluate(&scores[0]);
}

struct PotentialKeys CrackSBX(struct Buffer* buf, Analyzer analyzer, bool verbose) {
    // Single threaded implementation
    return CrackSBX_SingleThread(buf, analyzer, verbose);
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

struct PotentialKeys CrackSBX_Evaluate(int* scores) {
    // Find the top 5 values
    struct PotentialKeys results = {0};
    // Find the top 12 characters
    for (int i = 0; i < 256; i++) {
        uint8_t byte = (uint8_t) i;
        int count = scores[i];
        for (int j = 0; j < 5; j++) {
            bool is_larger = count > results.scores[j];
            // index 0 has the biggest max, set it unconditionally
            if (((j == 0) && is_larger) ||
                (is_larger && (count <= results.scores[j-1]) && (byte != results.keys[j-1]))) {
                    // Shift the array to make space for this new max value
                    for(int k = 4; k > j; k--) {
                        results.scores[k] = results.scores[k-1];
                        results.keys[k] = results.keys[k-1];
                    }
                results.scores[j] = (float) count;
                results.keys[j] = byte;
            }
        }
    }
    return results;
}
