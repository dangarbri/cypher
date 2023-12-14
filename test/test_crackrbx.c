#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "analyzers/english.h"
#include "crackers/crackrbx.h"
#include "parsers/hex.h"

int main() {
    // "Hello, this is a message from Bob. I am your superior, you must respect me."
    // Key: ICE
    char* cipher = "012629252c6969372d2030652030652863282c3036282420692537262e650b2c2767630c692228693a2a3c31653a36352c312c263169693a2a3c63283c30316931203a33202a376524266b";
    struct Buffer* data = Hex_Decode(cipher);
    assert(data != NULL);
    struct RepeatingXorKeys* keys = CrackRepeatingXor(1, 40, data, English_Analyzer, true);
    assert(keys != NULL);
    struct Buffer* key = keys->keys[0];
    assert(strncmp("ICE", (char*) key->data, 3) == 0);
    CrackRBX_FreeKeys(keys);
    Buffer_Free(data);
}