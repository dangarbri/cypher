#ifndef _ENGLISH_H_
#define _ENGLISH_H_

#include "types/buffer.h"

/**
 * This analyzes a buffer to determine if contains english text.
 * @param buffer Buffer to analyze
 * @return Score of how likely it is that this buffer has english plaintext
 *         TODO: Add scoring details
 */
float English_Analyzer(struct Buffer* buffer);

#endif