#include "hamming.h"

size_t GetHammingDistance(struct Buffer* a, struct Buffer* b) {
    if ((a == NULL) || (b == NULL) || (a->length != b->length)) {
        return SIZE_MAX;
    }

    // xor leaves 1's where the bits are different
    size_t count = 0;
    for(size_t i = 0; i < a->length; i++) {
        uint8_t a_xor_b = a->data[i] ^ b->data[i];
        count += (size_t) GetHammingWeight(a_xor_b);
    }
    return count;
}

/**
 * Counts the number of set bits in the given number
 *
 * @note Pulled from cpython's implementation
 * https://github.com/python/cpython/blob/a3a1cb48456c809f7b1ab6a6ffe83e8b3f69be0f/Modules/mathmodule.c#L1480
 */
uint8_t GetHammingWeight(uint8_t n)
{
    uint8_t count = 0;
    while (n != 0) {
        ++count;
        n &= n - 1; /* clear least significant bit */
    }
    return count;
}