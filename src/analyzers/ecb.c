#include <string.h>

#include "ecb.h"

float ECB_Analyzer(struct Buffer* buffer, size_t blocksize) {
    if (!Buffer_IsValid(buffer)) { return 0.0f; }
    if (buffer->length < blocksize) { return 0.0f; }
    size_t n_blocks = buffer->length / blocksize;
    size_t dupe_count = 0;
    for (size_t i = 0; i < (n_blocks - 1); i++) {
        uint8_t* left = buffer->data + (i * blocksize);
        for (size_t j = i + 1; j < n_blocks; j++) {
            uint8_t* right = buffer->data + (j * blocksize);
            if (memcmp(left, right, blocksize) == 0) {
                dupe_count += 1;
            }
        }
    }

    // 5 points for each duplicate block, up to 100 points
    float dupe_score = (dupe_count > 20 ? 20.0f : (float) dupe_count) * 0.05f;

    return dupe_score;
}

float ECB128_Analyzer(struct Buffer* buffer) {
    return ECB_Analyzer(buffer, 16);
}

float ECB256_Analyzer(struct Buffer* buffer) {
    return ECB_Analyzer(buffer, 32);
}