#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "operations/rank.h"

void test_RankInt() {
    int array[] = {4, 99, 3, 7, 56, 101};
    size_t out[6];
    size_t expected[] = {5, 1, 4, 3, 0, 2};
    int result = RankInt(out, 6, array, 6);
    assert(result == EXIT_SUCCESS);
    for (int i = 0; i < 6; i++) {
        printf("%d: %zd == %zd\n", i, out[i], expected[i]);
        assert(out[i] == expected[i]);
    }
}

void test_RankFloat() {
    float array[] = {1.5f, 99.0f, 15.5f, 33.7f, 1.2f, 1.3f};
    size_t out[6];
    size_t expected[] = {1, 3, 2, 0, 5, 4};
    int result = RankFloat(out, 6, array, 6);
    assert(result == EXIT_SUCCESS);
    for (int i = 0; i < 6; i++) {
        printf("%d: %zd == %zd\n", i, out[i], expected[i]);
        assert(out[i] == expected[i]);
    }
}



int main() {
    test_RankInt();
    test_RankFloat();
}