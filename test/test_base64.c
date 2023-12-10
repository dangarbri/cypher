#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>

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

void test_Base64_Encode() {
    char* hello_world = "Hello, world!";
    char* expected_result = "SGVsbG8sIHdvcmxkIQ==";
    size_t length = strlen(expected_result);
    struct Base64Data result = Base64_Encode(hello_world, strlen(hello_world));
    char* encoded_str = (char*) result.data;
    assert(result.length == length);
    assert(strncmp(expected_result, encoded_str, length) == 0);
    free(result.data);
}

int main() {
    test_Base64_Decode();
    test_Base64_Encode();
    return 0;
}