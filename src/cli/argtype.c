#include "argtype.h"

struct Arg Argtype_Parse(char* arg) {
    struct Arg result = {
        .type = ARGTYPE_STRING,
        .arg = arg
    };
    return result;
}
