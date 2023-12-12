#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "analyzers/english.h"
#include "crackers/cracksbx.h"
#include "parsers/hex.h"
#include "operations/sbxor.h"

int main() {
    char* test_data = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";
    struct HexData blob = Hex_Decode(test_data);
    assert(blob.valid);
    struct Buffer buf = {
        .data = blob.data,
        .length = blob.length
    };
    struct PotentialKeys keys = CrackSBX(&buf, English_Check, false);
    printf("Potential Keys:\n");
    for (int i = 0; i < 5; i++) {
        struct XorData pt = SBX_Perform(keys.keys[i], buf.data, buf.length);
        assert(pt.data != NULL);
        printf("  0x%02X | Score %f | Message: %s\n", keys.keys[i], keys.scores[i], pt.data);
        free(pt.data);
    }
    free(blob.data);
}