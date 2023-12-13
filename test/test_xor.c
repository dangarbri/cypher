#include <assert.h>
#include <string.h>
#include "../src/operations/xor.h"

int main() {
    char* a = "Hello";
    char result[5];
    struct Buffer result_buf = {
        .data = (uint8_t*) result,
        .length = 5
    };
    struct Buffer left_buf = {
        .data = (uint8_t*) a,
        .length = 5
    };
    enum XorResult xor_result = xor(&result_buf, &left_buf, &left_buf);
    assert(xor_result == XOR_SUCCESS);
    for (int i = 0; i < 5; i++) {
        assert(result[i] == 0);
    }
    xor_result = xor(&result_buf, &result_buf, &left_buf);
    assert(xor_result == XOR_SUCCESS);
    assert(strncmp(result, a, 5) == 0);
}