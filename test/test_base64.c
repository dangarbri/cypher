#undef NDEBUG
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>

#include "../src/parsers/base64.h"

void test_Base64_Decode() {
    char* hello_world = "SGVsbG8sIHdvcmxkIQ==";
    char* expected_result = "Hello, world!";
    size_t length = strlen(expected_result);
    struct Buffer* result = Base64_Decode(hello_world);
    char* decoded_str = (char*) result->data;
    assert(result->length == length);
    assert(strncmp(expected_result, decoded_str, length) == 0);
    Buffer_Free(result);
}

void test_Base64_Encode() {
    char* hello_world = "Hello, world!";
    char* expected_result = "SGVsbG8sIHdvcmxkIQ==";
    size_t length = strlen(expected_result);
    struct Buffer* result = Base64_Encode((uint8_t*) hello_world, strlen(hello_world));
    char* encoded_str = (char*) result->data;
    assert(result->length == length);
    assert(strncmp(expected_result, encoded_str, length) == 0);
    Buffer_Free(result);
}

int main() {
    test_Base64_Decode();
    test_Base64_Encode();
    return 0;
}
