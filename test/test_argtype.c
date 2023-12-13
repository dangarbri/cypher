#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "cli/argtype.h"

void test_Argtype_Parse() {
    char* basic = "Hello";
    struct Arg* arg = Argtype_New(basic);
    assert(arg != NULL);
    assert(arg->type == ARGTYPE_STRING);
    assert(basic == (char*) arg->buffer.data);
    Argtype_Free(arg);
}

void test_Argtype_Parse_Hex() {
    char hex[] = "hex:1234";
    struct Arg* arg = Argtype_New(hex);
    assert(arg != NULL);
    assert(arg->type == ARGTYPE_BINARY);
    assert(arg->buffer.length == 2);
    assert(arg->buffer.data[0] == 0x12);
    assert(arg->buffer.data[1] == 0x34);
    Argtype_Free(arg);
}

void test_Argtype_Parse_Base64() {
    // "Hello"
    char base64[] = "base64:SGVsbG8=";
    struct Arg* arg = (struct Arg*) Argtype_New(base64);
    assert(arg != NULL);
    assert(arg->type == ARGTYPE_BINARY);
    assert(arg->buffer.length == 5);
    assert(strncmp((char*) arg->buffer.data, "Hello", 5) == 0);
    Argtype_Free((struct Arg*) arg);
}

void test_Argtype_Parse_File() {
    struct Arg* arg = Argtype_New("file:test_data/cypher");
    assert(arg != NULL);
    assert(arg->type == ARGTYPE_BINARY);
    assert(arg->buffer.length == 14);
    assert(strncmp("Hello, cypher\n", arg->buffer.data, 14) == 0);
    Argtype_Free(arg);
}

int main() {
    test_Argtype_Parse();
    test_Argtype_Parse_Hex();
    test_Argtype_Parse_Base64();
    test_Argtype_Parse_File();
    return 0;
}