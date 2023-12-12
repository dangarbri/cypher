#ifndef _ANALYZER_SBX_H_
#define _ANALYZER_SBX_H_

#include "analyzer.h"

/**
 * This analyzes a buffer to determine if it is likely "encrypted"
 * with a single byte xor cipher.
 *
 * In particular, this will score the plaintext via analyzer for
 * every potential key. The result will be the maximum score of all
 * plaintexts returned by the analyzer. Meaning if the analyzer
 * doesn't detect what you're looking for, then the score will be low.
 * But if it does, then the score will be higher.
 * @param buffer Buffer to analyze.
 * @param analyzer Plaintext analyzer.
 */
float SBX_Analyzer(struct Buffer* buffer, Analyzer analyzer);

#endif
