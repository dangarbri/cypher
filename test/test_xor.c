#include <assert.h>
#include <string.h>
#include "../src/operations/xor.h"

int main() {
    char* a = "Hello";
    char result[5];
    xor((uint8_t*) result, (uint8_t*) a, (uint8_t*) a, 5);
    for (int i = 0; i < 5; i++) {
        assert(result[i] == 0);
    }
    xor((uint8_t*) result, (uint8_t*) result, (uint8_t*) a, 5);
    assert(strncmp(result, a, 5) == 0);
}