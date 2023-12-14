#include <assert.h>
#include <string.h>

#include "cli/argtype.h"
#include "operations/aes.h"

void test_AesEncrypt() {
    char KEY[] = "YELLOW SUBMARINE";
    struct Arg* key = Argtype_New(KEY);
    assert(key != NULL);
    char ciphertext[] = "file:test_data/cypher_enc";
    struct Arg* expected_result = Argtype_New(ciphertext);
    assert(expected_result != NULL);
    char plaintext[] = "file:test_data/cypher";
    struct Arg* pt = Argtype_New(plaintext);
    assert(pt != NULL);
    struct Buffer* encrypted = Aes128Ecb_Encrypt(&key->buffer, &pt->buffer);
    assert(encrypted != NULL);
    assert(encrypted->length == expected_result->buffer.length);
    assert(memcmp(encrypted->data, expected_result->buffer.data, expected_result->buffer.length) == 0);
    Argtype_Free(key);
    Argtype_Free(expected_result);
    Argtype_Free(pt);
    Buffer_Free(encrypted);
}

void test_AesDecrypt() {
    char KEY[] = "YELLOW SUBMARINE";
    struct Arg* key = Argtype_New(KEY);
    assert(key != NULL);
    char ciphertext[] = "file:test_data/cypher_enc";
    struct Arg* ct = Argtype_New(ciphertext);
    assert(ct != NULL);
    char plaintext[] = "file:test_data/cypher";
    struct Arg* pt = Argtype_New(plaintext);
    assert(pt != NULL);
    struct Buffer* decrypted = Aes128Ecb_Decrypt(&key->buffer, &ct->buffer);
    assert(decrypted != NULL);
    assert(decrypted->length == pt->buffer.length);
    assert(memcmp(decrypted->data, pt->buffer.data, pt->buffer.length) == 0);
    Argtype_Free(key);
    Argtype_Free(ct);
    Argtype_Free(pt);
    Buffer_Free(decrypted);
}

void test_AesDecryptGarbage() {
    char KEY[] = "YELLOW SUBMARINE";
    struct Arg* key = Argtype_New(KEY);
    assert(key != NULL);
    char plaintext[] = "file:test_data/cypher";
    struct Arg* pt = Argtype_New(plaintext);
    assert(pt != NULL);
    // Attempt to decrypt the plaintext, which should fail
    struct Buffer* decrypted = Aes128Ecb_Decrypt(&key->buffer, &pt->buffer);
    assert(decrypted == NULL);
    Argtype_Free(key);
    Argtype_Free(pt);
}

int main() {
    test_AesEncrypt();
    test_AesDecrypt();
    test_AesDecryptGarbage();
}