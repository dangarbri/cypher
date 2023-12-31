#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../src/analyzers/english.h"

int main() {
    char* message = "Hello, world! My name is bob";
    struct Buffer buf = {
        .data = (uint8_t*) message,
        .length = strlen(message)
    };
    float score = English_Analyzer(&buf);
    printf("Score: %f\n", score);
    assert(score >= 0.599 && score <= 0.601);
}