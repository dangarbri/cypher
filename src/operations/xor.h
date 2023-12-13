#ifndef _XOR_H_
#define _XOR_H_

#include <stddef.h>
#include <stdint.h>

#include "types/buffer.h"

enum XorResult {
    XOR_SUCCESS,
    /** Returned when buffer sizes don't match for the xor operation */
    XOR_INCORRECT_BUFFER_SIZE,
};

/**
 * XORs 2 binary buffers.
 * You must verify both buffers are the same length, otherwise data will be corrupted.
 * @param result Buffer where the xor result is stored
 * @param a first operand
 * @param b second operand
 * @param length Number of bytes to xor
 * @return XOR_SUCCESS on success, else an error code value > 0
 */
enum XorResult xor(struct Buffer* result, struct Buffer* a, struct Buffer* b);

/**
 * Perform a single byte xor operation
 * All bytes in data will be xor'd with the given key byte
 *
 * You must free the resultant buffer with Buffer_Free
 *
 * @param byte XOR key
 * @param data Buffer to xor
 * @return New buffer, data may be NULL if memory allocation fails
 */
struct Buffer* sb_xor(uint8_t key, struct Buffer* data);

#endif
