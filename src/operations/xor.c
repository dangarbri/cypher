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


enum XorResult sb_xor(struct Buffer* result, uint8_t key, struct Buffer* data) {
    if (result->length != data->length) {
        return XOR_INCORRECT_BUFFER_SIZE;
    }
    for (size_t i = 0; i < data->length; i++) {
        result->data[i] = data->data[i] ^ key;
    }
    return XOR_SUCCESS;
}