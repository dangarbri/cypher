#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/cli/argtype.h"
#include "../src/operations/xor.h"

void test_Xor() {
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

void test_RepeatingXor() {
    char* message = "Burning 'em, if you ain't quick and nimble";
    char expected[] = "hex:0b3637272a2b2e63622c2e69692a23693a2a3c6324202d623d63343c2a26226324272765272a282b2f20";
    struct Buffer* message_data = Argtype_New(message);
    assert(message_data != NULL);
    struct Buffer* expected_data = Argtype_New(expected);
    assert(expected_data != NULL);
    assert(message_data->length == expected_data->length);
    struct Buffer* result = Buffer_New(message_data->length);
    assert(result != NULL);
    struct Buffer key = {
        .data = (uint8_t*) "ICE",
        .length = 3
    };
    assert(message_data != NULL);
    assert(rp_xor(result, &key, message_data) == XOR_SUCCESS);
    assert(memcmp(expected_data->data, result->data, result->length) == 0);
    Buffer_Free(result);
    Argtype_Free(expected_data);
    Argtype_Free(message_data);
}

int main() {
    test_Xor();
    test_RepeatingXor();
}
