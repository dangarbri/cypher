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
    float score = English_Check(&buf);
    printf("Score: %f\n", score);
    assert(score > 0.799 && score < 0.801);
}