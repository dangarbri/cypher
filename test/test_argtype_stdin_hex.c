#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "cli/argtype.h"

int main() {
    char arg[] = "hex:-";
    struct Buffer* text = Argtype_New(arg);
    assert(text != NULL);
    printf("Entered: %s\n", (char*) text->data);
    assert(strcmp((char*) text->data, "hello") == 0);
}