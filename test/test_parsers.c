#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../src/parsers/hex.h"

void test_Hex_Parse() {
    char* data ="00010203040506070809";
    struct HexData hex = Hex_Parse(data);
    assert(hex.length == 10);
    for (int i = 0; i < 10; i++) {
        assert(hex.data[i] == i);
    }
    free(hex.data);
}

int main() {
    test_Hex_Parse();
    return 0;
}