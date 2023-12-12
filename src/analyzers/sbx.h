#ifndef _ANALYZER_SBX_H_
#define _ANALYZER_SBX_H_

#include "analyzer.h"

/**
 * This analyzes a buffer to determine if it is likely "encrypted"
 * with a single byte xor cipher.
 *
 * In particular, this will score the plaintext via the english analyzer
 * for every byte key.
 * @param buffer Buffer to analyze.
 */
float SBX_EnglishAnalyzer(struct Buffer* buffer);

#endif
