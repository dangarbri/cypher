#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "analyzers/english.h"
#include "crackers/cracksbx.h"
#include "parsers/hex.h"
#include "operations/xor.h"

int main() {
    char* test_data = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";
    struct Buffer* blob = Hex_Decode(test_data);
    assert(blob != NULL);
    struct PotentialKeys keys = CrackSBX(blob, English_Analyzer, false);
    printf("Potential Keys:\n");
    struct Buffer* pt = Buffer_New(blob->length);
    if (pt != NULL) {
        for (int i = 0; i < 5; i++) {
            if (sb_xor(pt, keys.keys[i], blob) == XOR_SUCCESS) {
                assert(pt != NULL);
                printf("  0x%02X | Score %f | Message: %s\n", keys.keys[i], keys.scores[i], pt->data);
            }
        }
        Buffer_Free(pt);
    }
    Buffer_Free(blob);
}