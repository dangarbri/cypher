#ifndef _CRACK_RBX_H_
#define _CRACK_RBX_H_

#include "analyzers/analyzer.h"

#define CRACKRBX_MAX_KEYS 5

struct RankedKeySize {
    size_t keysize[CRACKRBX_MAX_KEYS];
};

/**
 * List of keys
 */
struct RepeatingXorKeys {
    /** Array of pointers to buffers containing keys */
    struct Buffer** keys;
    /** Number of items in the key list*/
    size_t length;
};

/**
 * Attempts to crack a repeating xor cypher via the given analyzer
 * @note This function allocates memory. Free the result with CrackRBX_FreeKeys()
 * @param keymin Minimum key size to test
 * @param keymax Maximum key size to test
 * @param buf ciphertext buffer
 * @param analyzer Analyzer to use for scoring plaintext
 * @param verbose Print information to stdout
 * @returns Likely Repeating XOR keys, or NULL on failure
 */
struct RepeatingXorKeys* CrackRepeatingXor(size_t keymin, size_t keymax, struct Buffer* buf, Analyzer analyzer, bool verbose);

/**
 * Attempts to crack repeating xor with the given key length
 * Returns the most likely key in a buffer.
 * @note This function allocates memory, free the result with Buffer_Free
 * @return key buffer or NULL
 */
struct Buffer* CrackRepeatingXor_WithKeyLength(size_t keylength, const struct Buffer* buf, Analyzer analyzer, bool verbose);

/**
 * Ranks all key lengths between keymin and keymax (inclusive) on buffer buf
 * and stores the result in out.
 * @param out Output struct to store the result
 * @param keymin minimum key length. Must be > 0 or this function will fail.
 * @param keymax maximum key length. Must be > keymin.
 * @param buf Buffer to analyze
 * @returns EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 */
int CrackRBX_RankKeySizes(struct RankedKeySize* out, size_t keymin, size_t keymax, struct Buffer* buf);

/**
 * Frees memory allocated by the cracking process
 */
void CrackRBX_FreeKeys(struct RepeatingXorKeys* keys);

/**
 * Prints a summary of the given keys to stdout
 */
void CrackRBX_PrintSummary(struct RepeatingXorKeys* keylist);

#endif