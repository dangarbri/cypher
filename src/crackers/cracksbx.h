#ifndef _CRACKSBX_H_
#define _CRACKSBX_H_

#include <stdio.h>
#include "analyzers/analyzer.h"

struct PotentialKeys {
    uint8_t keys[5];
    float scores[5];
};

/**
 * This module cracks a single byte xor cipher.
 * The key ranking is chosen based on the given analyzer
 * @param buf Buffer pointing to ciphertext.
 * @param analyzer Analyzer to use to score potential keys.
 * @param verbose If true, progress is printed to stdout and all permutations are saved in an autogenerated folder.
 * @return array of top 5 most likely keys
 */
struct PotentialKeys CrackSBX(struct Buffer* buf, Analyzer analyzer, bool verbose);

/**
 * Prints the given keys to stdout
 */
void CrackSBX_PrintKeys(struct PotentialKeys* keys, FILE* stream);

#endif