#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../src/parsers/hex.h"

void test_Hex_Decode() {
    char* data ="00010203040506070809";
    struct HexData hex = Hex_Decode(data);
    assert(hex.length == 10);
    for (int i = 0; i < 10; i++) {
        assert(hex.data[i] == i);
    }
    free(hex.data);
}

void test_Hex_Decode_Invalid() {
    char* data = "12345678GGGG";
    struct HexData hex = Hex_Decode(data);
    assert(hex.valid == false);
}

void test_Hex_Encode() {
    char* hello = "Hello";
    char* result = Hex_Encode(hello, 5);
    assert(result != NULL);
    assert(strlen(result) == 10);
    assert(strncmp(result, "48656c6c6f", 10) == 0);
    free(result);
}

int main() {
    test_Hex_Decode();
    test_Hex_Decode_Invalid();
    test_Hex_Encode();
    return 0;
}