#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../src/base64/base64.h"

void test_Base64_Decode() {
    char* hello_world = "SGVsbG8sIHdvcmxkIQ==";
    char* expected_result = "Hello, world!";
    size_t length = strlen(expected_result);
    struct Base64Data result = Base64_Decode(hello_world);
    char* decoded_str = (char*) result.data;
    assert(result.length == length);
    assert(strncmp(expected_result, decoded_str, length) == 0);
    free(result.data);
}

int main() {
    test_Base64_Decode();
    return 0;
}