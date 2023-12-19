#undef NDEBUG
#include <assert.h>
#include <stdio.h>
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
#ifdef _MSC_VER
    // Windows \r adds an extra character
    assert(arg->length == 47);
#else
    assert(arg->length == 46);
#endif
    assert(strncmp("Hello, cypher! This spans is 3 128-bit blocks\n", (char*) arg->data, 45) == 0);
    Argtype_Free(arg);
}

void test_Argtype_Program() {
#ifdef _MSC_VER
    char cmd[] = "prog:dir";
#else
    char cmd[] = "prog:ls -l";
#endif
    struct Buffer* data = Argtype_New(cmd);
    assert(data != NULL);
    printf("%s", data->data);
    Buffer_Free(data);
}

int main() {
    test_Argtype_Parse();
    test_Argtype_Parse_Hex();
    test_Argtype_Parse_Base64();
    test_Argtype_Parse_File();
    test_Argtype_Program();
    return 0;
}