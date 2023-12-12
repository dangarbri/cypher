#ifndef _XOR_H_
#define _XOR_H_

#include <stddef.h>
#include <stdint.h>

/**
 * XORs 2 binary buffers.
 * You must verify both buffers are the same length, otherwise data will be corrupted.
 * @param result Buffer where the xor result is stored
 * @param a first operand
 * @param b second operand
 * @param length Number of bytes to xor
 */
void xor(uint8_t* result, const uint8_t* a, const uint8_t* b, size_t length);

struct XorData {
    uint8_t* data;
    size_t length;
};

/**
 * Perform a single byte xor operation
 * All bytes in data will be xor'd with the given key byte
 *
 * You must free the resultant buffer
 *
 * @param byte XOR key
 * @param data Buffer to xor
 * @param length Size of buffer
 * @return XorData, data may be NULL if memory allocation fails
 */
struct XorData sb_xor(uint8_t key, uint8_t* data, size_t length);

#endif
