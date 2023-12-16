#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/operations/xor.h"

int main() {
    struct Buffer* testbuf = Buffer_New(20);
    assert(testbuf != NULL);
    memset(testbuf->data, 0, 20);
    struct Buffer* result = Buffer_New(20);
    assert(result != NULL);
    enum XorResult xor_result = sb_xor(result, 0x0a, testbuf);
    assert(xor_result == XOR_SUCCESS);
    (void)xor_result;
    assert(result != NULL);
    assert(result->length == 20);
    for (int i = 0; i < 20; i++) {
        assert(result->data[i] == 0x0a);
    }
    Buffer_Free(testbuf);
    Buffer_Free(result);
}