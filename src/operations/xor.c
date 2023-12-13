#include <stdio.h>
#include <stdlib.h>

#include "xor.h"

enum XorResult xor(struct Buffer* result, struct Buffer* a, struct Buffer* b) {
    if ((a->length != b->length) || (result->length != a->length)) {
        return XOR_INCORRECT_BUFFER_SIZE;
    }
    for (size_t i = 0; i < a->length; i++) {
        result->data[i] = a->data[i] ^ b->data[i];
    }
    return XOR_SUCCESS;
}


struct Buffer* sb_xor(uint8_t key, struct Buffer* buffer) {
    struct Buffer* result = Buffer_New(buffer->length);
    if (result->data != NULL) {
        for (size_t i = 0; i < buffer->length; i++) {
            result->data[i] = buffer->data[i] ^ key;
        }
    } else {
        fputs("sb_xor: Failed to allocate memory for buffer\n", stderr);
    }
    return result;
}