#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "cli/argtype.h"

void test_Argtype_Parse() {
    char* basic = "Hello";
    struct Buffer* arg = Argtype_New(basic);
    assert(arg != NULL);
    assert(basic != (char*) arg->data);
    Argtype_Free(arg);
}

void test_Argtype_Parse_Hex() {
    char hex[] = "hex:1234";
    struct Buffer* arg = Argtype_New(hex);
    assert(arg != NULL);
    assert(arg->length == 2);
    assert(arg->data[0] == 0x12);
    assert(arg->data[1] == 0x34);
    Argtype_Free(arg);
}

void test_Argtype_Parse_Base64() {
    // "Hello"
    char base64[] = "base64:SGVsbG8=";
    struct Buffer* arg = (struct Buffer*) Argtype_New(base64);
    assert(arg != NULL);
    assert(arg->length == 5);
    assert(strncmp((char*) arg->data, "Hello", 5) == 0);
    Argtype_Free((struct Buffer*) arg);
}

void test_Argtype_Parse_File() {
    char fname[] = "file:test_data/cypher";
    struct Buffer* arg = Argtype_New(fname);
    assert(arg != NULL);
    assert(arg->length == 14);
    assert(strncmp("Hello, cypher\n", (char*) arg->data, 14) == 0);
    Argtype_Free(arg);
}

int main() {
    test_Argtype_Parse();
    test_Argtype_Parse_Hex();
    test_Argtype_Parse_Base64();
    test_Argtype_Parse_File();
    return 0;
}