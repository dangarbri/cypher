#include <assert.h>
#include "../src/cli/argtype.h"

void test_Argtype_Parse() {
    char* basic = "Hello";
    struct Arg arg = Argtype_Parse(basic);
    assert(arg.type == ARGTYPE_STRING);
    assert(basic == (char*) arg.arg);
}

// void test_Argtype_Parse_Hex() {
//     char* hex = "hex:1234";
//     struct Arg arg = Argtype_Parse(hex);
//     assert(arg.type == ARGTYPE_HEX);
// }

int main() {
    test_Argtype_Parse();
    // test_Argtype_Parse_Hex();
    // test_Argtype_Parse_Base64();
    return 0;
}