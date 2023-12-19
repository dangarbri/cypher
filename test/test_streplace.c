#undef NDEBUG
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "strings/replace.h"

void test_streplace_start() {
    char test[] = "FUZZ test";
    char* expected_result = "This is a test";
    char* result = streplace(test, "FUZZ", "This is a");
    assert(result != NULL);
    assert(strcmp(result, expected_result) == 0);
    free(result);
}

void test_streplace_middle() {
    char test[] = "This X test";
    char* expected_result = "This is a test";
    char* result = streplace(test, "X", "is a");
    assert(result != NULL);
    assert(strcmp(result, expected_result) == 0);
    free(result);
}

void test_streplace_end() {
    char test[] = "This FUZZ";
    char* expected_result = "This is a test";
    char* result = streplace(test, "FUZZ", "is a test");
    assert(result != NULL);
    assert(strcmp(result, expected_result) == 0);
    free(result);
}

int main() {
    test_streplace_start();
    test_streplace_middle();
    test_streplace_end();
}