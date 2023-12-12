#ifndef _FREQUENCY_H_
#define _FREQUENCY_H_

#include "types/buffer.h"

/**
 * Interface for analyzing the contents of a buffer and returning some score
 * evaluated by an implementation defined heuristic.
 *
 * In general this returns a value between 0 and 1 which describes how likely
 * the buffer matches an implementation-defined heuristic
 * @param buffer Buffer to analyze
 * @return score between 0 and 1
 */
typedef float (*Analyzer)(struct Buffer* buffer);

#endif