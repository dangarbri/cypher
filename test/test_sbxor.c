#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../src/operations/sbxor.h"

int main() {
    uint8_t* testbuf = malloc(20);
    memset(testbuf, 0, 20);
    struct XorData result = SBX_Perform(0x0a, testbuf, 20);
    assert(result.data != NULL);
    assert(result.length == 20);
    for (int i = 0; i < 20; i++) {
        assert(result.data[i] == 0x0a);
    }
}