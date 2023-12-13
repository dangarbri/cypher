#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "types/buffer.h"

void test_Transpose() {
    struct Buffer i = {
        .data = (uint8_t*) "123456789",
        .length = 9
    };
    struct Buffer* transposed = Buffer_Transpose(&i, 3);
    assert(transposed != NULL);
    assert(transposed->length == 9);
    assert(memcmp((char*) transposed->data, "147258369", 9) == 0);
    Buffer_Free(transposed);

    transposed = Buffer_Transpose(&i, 4);
    assert(transposed != NULL);
    assert(transposed->length == 8);
    assert(memcmp((char*) transposed->data, "15263748", 8) == 0);
    Buffer_Free(transposed);

    transposed = Buffer_Transpose(&i, 20);
    assert(transposed != NULL);
    assert(transposed->length == 9);
    assert(memcmp((char*) transposed->data, "123456789", 9) == 0);
    Buffer_Free(transposed);

    struct Buffer j = {
        .data = (uint8_t*) "123456789abcdefghi",
        .length = 18
    };
    transposed = Buffer_Transpose(&j, 3);
    assert(transposed != NULL);
    assert(transposed->length == 18);
    assert(memcmp((char*) transposed->data, "147adg258beh369cfi", 18) == 0);
    Buffer_Free(transposed);

    assert(Buffer_Transpose(NULL, 4) == NULL);
}

void test_New() {
    struct Buffer* i = Buffer_New(20);
    assert(i != NULL);
    assert(i->data != NULL);
    assert(i->length == 20);
    Buffer_Free(i);
}

int main() {
    test_New();
    test_Transpose();
}