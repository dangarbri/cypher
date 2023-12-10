#include <assert.h>
#include <string.h>
#include "../src/operations/xor.h"

int main() {
    char* a = "Hello";
    char result[5];
    xor(result, a, a, 5);
    for (int i = 0; i < 5; i++) {
        assert(result[i] == 0);
    }
    xor(result, result, a, 5);
    assert(strncmp(result, a, 5) == 0);
}