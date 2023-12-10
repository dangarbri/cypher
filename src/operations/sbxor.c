#include "sbxor.h"

struct XorData SBX_Perform(uint8_t key, uint8_t* data, size_t length) {
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
        perror("SBX_Perform: ");
    }
    return result;
}