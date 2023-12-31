#include <string.h>

#include "english.h"

struct FrequencyHistogram {
    // Allocate space for 0 thru 0xFF
    uint64_t counters[256];
    /** Number of letter/symbol glyphs (between 0x20 and 0x7F)*/
    uint64_t ascii_count;
    /** Number of lower case letters */
    uint64_t upper_count;
    /** Number of lower case letters */
    uint64_t lower_count;
    /** Number of symbols in the text */
    uint64_t symbol_count;
};

/**
 * Tries to determine if data pointed to by buffer is english plaintext
 * The heuristics used are as follows:
 *   (1 point = 0.01 or 1%)
 *   - 5 points for each top 12 occurring byte high frequency byte which is one of ETAOIN SHRDLU
 *   - 10 points if 100% of characters are alphanumeric + symbols or line feeds
 *   - 5 points (instead of 10) if 70% of characters are alphanumeric + symbols or line feeds
 *   - 10 points if symbols are < 10% of all bytes
 *   - 10 points if a majority of symbols are lower case
 */
float English_Analyzer(struct Buffer* buffer) {
    // Safety check.
    if (!Buffer_IsValid(buffer)) {
        return 0;
    }

    // Initialize counters to 0
    struct FrequencyHistogram histogram = {0};
    float score = 0;
    // Count byte frequency
    for (size_t i = 0; i < buffer->length; i++) {
        uint8_t byte = buffer->data[i];
        histogram.counters[byte] += 1;
        // 0x20 through 0x7E are alphanumeric and punctuation typically used in english text.
        // Below 0x20 are control characters, which should not be in english plaintext
        // Greater than 0x7F are atypical symbols (for english). Like the umlaut and
        // accented characters.
        if ((byte >= ' ' && byte <= '~') || (byte == '\n') || (byte == '\r')) {
            histogram.ascii_count += 1;
            if ((byte >= 'a' && byte <= 'z')) {
                histogram.lower_count += 1;
            } else if ((byte >= 'A' && byte <= 'Z')) {
                histogram.upper_count += 1;
            }
            if (((byte >= '!') && (byte <= '/')) || ((byte >= ':') && (byte <= '@')) || ((byte >= '[' && byte <= '`')) || (byte >= '{' && byte <= '~')) {
                histogram.symbol_count += 1;
            }
        }
    }

    // Apply points if data is all alphanumeric/symbolic.
    // 10 points
    if (buffer->length == histogram.ascii_count) {
        score += 0.10f;
    } else if (histogram.ascii_count > ((buffer->length * 7) / 10)) {
        score += 0.05f;
    }

    // 10 points
    if (histogram.symbol_count < ((buffer->length * 10) / 100)) {
        score += 0.10f;
    }

    // 10 points
    if (histogram.lower_count > histogram.upper_count) {
        score += 0.10f;
    }


    // Frequency analysis check.
    // 5 points per high frequency letter
    uint8_t max_bytes[12] = {0};
    uint64_t max_counts[12] = {0};
    // Find the top 12 characters
    for (int i = 0; i < 256; i++) {
        uint8_t byte = (uint8_t) i;
        uint64_t count = histogram.counters[i];
        for (int j = 0; j < 12; j++) {
            bool is_larger = count > max_counts[j];
            // index 0 has the biggest max, set it unconditionally
            if (((j == 0) && is_larger) ||
                (is_larger && (count <= max_counts[j-1]) && (byte != max_bytes[j-1]))) {
                    // Shift the array to make space for this new max value
                    for(int k = 11; k > j; k--) {
                        max_counts[k] = max_counts[k-1];
                        max_bytes[k] = max_bytes[k-1];
                    }
                max_counts[j] = count;
                max_bytes[j] = byte;
            }
        }
    }

    // High frequency characters
    char* hf_chars = "ETAOIN SHRDLUetaoinshrdlu";
    // Score based on these chars
    for (int i = 0; i < 12; i++) {
        if (strchr(hf_chars, max_bytes[i]) != NULL) {
            score += .05f;
        }
    }

    return score;
}
