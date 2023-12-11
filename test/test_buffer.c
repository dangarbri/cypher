#include <assert.h>
#include "../src/types/buffer.h"

int main() {
    struct Buffer* i = Buffer_New(20);
    assert(i != NULL);
    assert(i->data != NULL);
    assert(i->length == 20);
    Buffer_Free(i);
}