#include <stdio.h>

#include "analyze.h"

void AnalyzeUsage() {
    puts("Usage:");
    puts("  analyze <message>");
    puts("");
    puts("Analyze will analyze the given message and try to");
    puts("determine how it was encrypted. It is very likely");
    puts("that the analyzer will fail on modern encryption.");
}

int AnalyzeCmd(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    AnalyzeUsage();
    return 0;
}