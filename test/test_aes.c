#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "cli/argtype.h"
#include "operations/aes.h"

void test_AesEcbEncrypt() {
    char KEY[] = "YELLOW SUBMARINE";
    struct Buffer* key = Argtype_New(KEY);
    assert(key != NULL);
    char ciphertext[] = "file:test_data/cypher_ecb";
    struct Buffer* expected_result = Argtype_New(ciphertext);
    assert(expected_result != NULL);
    char plaintext[] = "file:test_data/cypher";
    struct Buffer* pt = Argtype_New(plaintext);
#ifdef _MSC_VER
    // Remove carriage return since expected results expects only \n
    char* carriage_return = strchr((const char*) pt->data, '\r');
    *carriage_return = '\n';
    *(carriage_return + 1) = '\0';
    pt->length -= 1;
#endif
    assert(pt != NULL);
    struct Buffer* encrypted = Aes128Ecb_Encrypt(key, pt);
    assert(encrypted != NULL);
    assert(encrypted->length == expected_result->length);
    assert(memcmp(encrypted->data, expected_result->data, expected_result->length) == 0);
    Argtype_Free(key);
    Argtype_Free(expected_result);
    Argtype_Free(pt);
    Buffer_Free(encrypted);
}

void test_AesEcbDecrypt() {
    char KEY[] = "YELLOW SUBMARINE";
    struct Buffer* key = Argtype_New(KEY);
    assert(key != NULL);
    char ciphertext[] = "file:test_data/cypher_ecb";
    struct Buffer* ct = Argtype_New(ciphertext);
    assert(ct != NULL);
    char plaintext[] = "file:test_data/cypher";
    struct Buffer* pt = Argtype_New(plaintext);
#ifdef _MSC_VER
    // Remove carriage return since expected results expects only \n
    char* carriage_return = strchr((const char*)pt->data, '\r');
    *carriage_return = '\n';
    *(carriage_return + 1) = '\0';
    pt->length -= 1;
#endif
    assert(pt != NULL);
    struct Buffer* decrypted = Aes128Ecb_Decrypt(key, ct);
    assert(decrypted != NULL);
    assert(decrypted->length == pt->length);
    assert(memcmp(decrypted->data, pt->data, pt->length) == 0);
    Argtype_Free(key);
    Argtype_Free(ct);
    Argtype_Free(pt);
    Buffer_Free(decrypted);
}

void test_AesEcbDecryptGarbage() {
    char KEY[] = "YELLOW SUBMARINE";
    struct Buffer* key = Argtype_New(KEY);
    assert(key != NULL);
    char plaintext[] = "file:test_data/cypher";
    struct Buffer* pt = Argtype_New(plaintext);
    assert(pt != NULL);
    // Attempt to decrypt the plaintext, which should fail
    struct Buffer* decrypted = Aes128Ecb_Decrypt(key, pt);
    assert(decrypted == NULL);
    Argtype_Free(key);
    Argtype_Free(pt);
}

void test_AesCbcEncryptDecrypt() {
    char KEY[] = "YELLOW SUBMARINE";
    struct Buffer* key = Argtype_New(KEY);
    assert(key != NULL);
    uint8_t iv[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10};
    struct Buffer iv_buf = {.data = iv, .length = 16};
    char plaintext_file[] = "file:test_data/cypher";
    struct Buffer* plaintext = Argtype_New(plaintext_file);
    struct Buffer* ciphertext = Aes128Cbc_Encrypt(key, &iv_buf, plaintext);
    assert(ciphertext != NULL);
    struct Buffer* plaintext2 = Aes128Cbc_Decrypt(key, &iv_buf, ciphertext);
    assert(plaintext2 != NULL);
    assert(memcmp(plaintext2->data, plaintext->data, plaintext->length) == 0);
    Argtype_Free(key);
    Argtype_Free(plaintext);
    Buffer_Free(ciphertext);
    Buffer_Free(plaintext2);
}

int main() {
    test_AesEcbEncrypt();
    test_AesEcbDecrypt();
    test_AesEcbDecryptGarbage();
    test_AesCbcEncryptDecrypt();
}