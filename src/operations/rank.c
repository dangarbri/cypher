#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rank.h"

#define RankX(type, out, n, array, length)                                   \
    for (size_t i = 0; i < length; i++) {                                    \
        type count = array[i];                                               \
        for (size_t j = 0; j < n; j++) {                                     \
            bool is_larger = count > values[j];                              \
            if (((j == 0) && is_larger) ||                                   \
                (is_larger && (count <= values[j-1]) && (i != out[j-1]))) {  \
                    for(size_t k = n-1; k > j; k--) {                        \
                        values[k] = values[k-1];                             \
                        out[k] = out[k-1];                                   \
                    }                                                        \
                values[j] = count;                                           \
                out[j] = i;                                                  \
            }                                                                \
        }                                                                    \
    }

#define _RankX(type, out, n, array, length) \
    if ((out == NULL) || (array == NULL) || (n > length)) { \
        return EXIT_FAILURE;                \
    }                                       \
    type* values = calloc(n, sizeof(type)); \
    if (values == NULL) {                   \
        perror("Rank Failure");             \
        return EXIT_FAILURE;                \
    }                                       \
    memset(out, 0, sizeof(size_t) * n);     \
    RankX(type, out, n, array, length);     \
    free(values);                           \
    return EXIT_SUCCESS;


int RankInt(size_t* out, size_t n, const int* array, size_t length) {
    _RankX(int, out, n, array, length);
}

int RankFloat(size_t* out, size_t n, const float* array, size_t length) {
    _RankX(float, out, n, array, length);
}