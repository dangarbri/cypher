#include "analyzers/english.h"
#include "crackers/cracksbx.h"
#include "sbx.h"

float SBX_EnglishAnalyzer(struct Buffer* buffer) {
    // Use CrackSBX to score the keys.
    struct PotentialKeys keys = CrackSBX(buffer, English_Analyzer, false);
    // Return the top score.
    return keys.scores[0];
}