#ifndef _ECB_H_
#define _ECB_H_

#include "types/buffer.h"

/**
 * Analyzes the given buffer to see if it may be encrypted
 * via ECB block mode.
 *
 * Remember that the problem with ECB is that it is stateless and deterministic.
 * The same 16 byte plaintext block will always produce the same 16 byte ciphertext.
 * This analyzer scans for duplicate blocks in a given cyphertext.
 *
 * @param buffer data to analyze
 * @returns score between 0 and 1
 */
float ECB128_Analyzer(struct Buffer* buffer);

/**
 * Analyzes the given buffer to see if it may be encrypted
 * via ECB block mode.
 */
float ECB256_Analyzer(struct Buffer* buffer);

#endif