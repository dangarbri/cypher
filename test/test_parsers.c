#undef NDEBUG
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../src/parsers/hex.h"

void test_Hex_Decode() {
    char* data ="00010203040506070809";
    struct Buffer* hex = Hex_Decode(data);
    assert(hex != NULL);
    assert(hex->length == 10);
    for (int i = 0; i < 10; i++) {
        assert(hex->data[i] == i);
    }
    Buffer_Free(hex);
}

void test_Hex_Decode_Invalid() {
    char* data = "12345678GGGG";
    struct Buffer* hex = Hex_Decode(data);
    assert(hex == NULL);
}

void test_Hex_Encode() {
    char* hello = "Hello";
    struct Buffer buf = {.data = (uint8_t*) hello, .length = 5};
    char* result = Hex_Encode(&buf);
    assert(result != NULL);
    assert(strlen(result) == 10);
    assert(strcmp(result, "48656c6c6f") == 0);
    free(result);
}

int main() {
    test_Hex_Decode();
    test_Hex_Decode_Invalid();
    test_Hex_Encode();
    return 0;
}