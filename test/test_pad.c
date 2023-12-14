#include <assert.h>
#include <string.h>
#include "cli/argtype.h"
#include "operations/pad.h"

int main() {
    char input[] = "YELLOW SUBMARINE";
    char expected[] = "YELLOW SUBMARINE\x04\x04\x04\x04";
    struct Arg* arg = Argtype_New(input);
    Pad(&arg->buffer, 20, Pad_PKCS7);
    assert(arg->buffer.length == 20);
    assert(memcmp(arg->buffer.data, expected, 20) == 0);
    Argtype_Free(arg);
}