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
    struct Buffer keybuf = {
        .data = &key,
        .length = 1
    };
    return rp_xor(result, &keybuf, data);
}

enum XorResult rp_xor(struct Buffer* result, struct Buffer* key, struct Buffer* data) {
    if (result->length != data->length) {
        return XOR_INCORRECT_BUFFER_SIZE;
    }

    for (size_t i = 0; i < data->length; i++) {
        uint8_t key_byte = key->data[i % key->length];
        result->data[i] = key_byte ^ data->data[i];
    }
    return XOR_SUCCESS;
}