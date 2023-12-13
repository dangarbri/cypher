#ifndef _HAMMING_H_
#define _HAMMING_H_

#include <stddef.h>
#include "types/buffer.h"

/**
 * Computes the hamming distance between two buffers.
 * @return the hamming distance or SIZE_MAX on failure.
 */
size_t GetHammingDistance(struct Buffer* a, struct Buffer* b);

/**
 * Computes the hamming weight of a byte
 * This is the number of 1 bits in the value
 */
uint8_t GetHammingWeight(uint8_t n);

#endif