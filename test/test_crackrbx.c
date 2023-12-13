#include <assert.h>
#include <stdlib.h>
#include "analyzers/english.h"
#include "crackers/crackrbx.h"
#include "parsers/hex.h"

int main() {
    // "Hello, this is a message from Bob. I am your superior, you must respect me."
    char* cipher = "012629252c6969372d2030652030652863282c3036282420692537262e650b2c2767630c692228693a2a3c31653a36352c312c263169693a2a3c63283c30316931203a33202a376524266b";
    struct HexData data = Hex_Decode(cipher);
    assert(data.valid);
    struct Buffer buf = {
        .data = data.data,
        .length = data.length
    };
    struct RepeatingXorKeys* keys = CrackRepeatingXor(1, 40, &buf, English_Analyzer, true);
    CrackRBX_FreeKeys(keys);
    free(data.data);
}