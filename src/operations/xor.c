#include "xor.h"

void xor(uint8_t* result, const uint8_t* a, const uint8_t* b, size_t length) {
    for (size_t i = 0; i < length; i++) {
        result[i] = a[i] ^ b[i];
    }
}