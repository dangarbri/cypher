#ifndef _FREQUENCY_H_
#define _FREQUENCY_H_

#include "types/buffer.h"

/**
 * Interface for frequency analysis functions.
 * A frequency analyzer takes a buffer
 * Value between 0 and 1 which describes how likely
 * the buffer matches an implementation-defined heuristic
 * @param buffer Buffer to analyze
 * @return score between 0 and 1
 */
typedef float (*FrequencyAnalyzer)(struct Buffer buffer);

#endif