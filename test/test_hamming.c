#undef NDEBUG
#include <assert.h>

#include "operations/hamming.h"

int main() {
    struct Buffer a = {
        .data = (uint8_t*) "this is a test",
        .length = 14
    };
    struct Buffer b = {
        .data = (uint8_t*) "wokka wokka!!!",
        .length = 14
    };
    size_t distance = GetHammingDistance(&a, &b);
    assert(distance == 37);
}