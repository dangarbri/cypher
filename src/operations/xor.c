#include <stdio.h>
#include <stdlib.h>

#include "xor.h"

void xor(uint8_t* result, const uint8_t* a, const uint8_t* b, size_t length) {
    for (size_t i = 0; i < length; i++) {
        result[i] = a[i] ^ b[i];
    }
}


struct XorData sb_xor(uint8_t key, uint8_t* data, size_t length) {
    struct XorData result = {
        .data = NULL,
        .length = 0
    };
    result.data = malloc(length);
    if (result.data != NULL) {
        result.length = length;
        for (size_t i = 0; i < length; i++) {
            result.data[i] = data[i] ^ key;
        }
    } else {
        perror("sb_xor: ");
    }
    return result;
}