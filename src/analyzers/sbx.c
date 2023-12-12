#include "crackers/cracksbx.h"
#include "sbx.h"

float SBX_Analyzer(struct Buffer* buffer, Analyzer analyzer) {
    // Use CrackSBX to score the keys.
    struct PotentialKeys keys = CrackSBX(buffer, analyzer, false);
    // Return the top score.
    return keys.scores[0];
}