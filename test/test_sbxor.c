#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/operations/xor.h"

int main() {
    struct Buffer* testbuf = Buffer_New(20);
    assert(testbuf != NULL);
    memset(testbuf->data, 0, 20);
    struct Buffer* result = sb_xor(0x0a, testbuf);
    (void)result;
    assert(result != NULL);
    assert(result->length == 20);
    for (int i = 0; i < 20; i++) {
        assert(result->data[i] == 0x0a);
    }
    Buffer_Free(testbuf);
    Buffer_Free(result);
}