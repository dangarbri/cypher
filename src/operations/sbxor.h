#ifndef _SBXOR_H_
#define _SBXOR_H_

#include <stddef.h>
#include <stdint.h>

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
struct XorData SBX_Perform(uint8_t key, uint8_t* data, size_t length);

#endif