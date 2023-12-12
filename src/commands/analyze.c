#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "analyze.h"
#include "analyzers/english.h"
#include "analyzers/sbx.h"
#include "cli/argtype.h"

struct RegisteredAnalyzer {
    const char* name;
    Analyzer fn;
};

struct RegisteredAnalyzer RegisteredAnalyzers[] = {
    {"English", English_Analyzer},
    {"Single Byte Xor", SBX_EnglishAnalyzer},
    {NULL}
};

void AnalyzeHelp() {
    puts("This command runs all the analyzers that cypher knows\n"
         "In the left column is the name of the analyzer.\n"
         "In the right is the score that the analyzer returned.\n"
         "Analyzers return a value between 0 and 1.\n"
         "A value closer to 0 is less likely to be true for that analyer\n"
         "and a value closer to 1 is more likely.\n"
         "IMPORTANT!!! These are not probabilities!!!\n"
         "The analyzers score based on their own set of heuristics\n"
         "and the scoring mechanism is completely arbitrary, the interface\n"
         "only requires that the result is clamped to a value between 0 and 1.\n"
         "\n"
         "For example this output:\n"
         "    Single Byte Xor | 0.7000\n"
         "\n"
         "Means that the the single byte xor analyzer scored the input\n"
         "at 0.7. As this value is closer to 1, it suggests that you would\n"
         "likely find some plaintext by cracking the cipher with the\n"
         "crack utility.");
}

void AnalyzeUsage() {
    puts("Usage:");
    puts("  analyze [-h] <message>");
    puts("");
    puts("Analyze will analyze the given message and try to");
    puts("determine how it was encrypted. It is very likely");
    puts("that the analyzer will fail on modern encryption.");
    puts("");
    puts("Options:");
    puts("  -h     Prints more detailed help");
}

int Analyze(char* message) {
    int result = EXIT_FAILURE;
    struct Arg* arg = Argtype_New(message);
    if (arg != NULL) {
        struct RegisteredAnalyzer* analyzer = &RegisteredAnalyzers[0];
        while (analyzer->fn != NULL) {
            float score = analyzer->fn(&arg->buffer);
            printf("%-15s | %.04f\n", analyzer->name, score);
            analyzer += 1;
        }
        result = EXIT_SUCCESS;
        Argtype_Free(arg);
    }
    return result;
}

int AnalyzeCmd(int argc, char* argv[]) {
    if ((argc > 1) && (strncmp(argv[1], "-h", 2) == 0)) {
        AnalyzeHelp();
        return EXIT_SUCCESS;
    }
    if (argc == 2) {
        return Analyze(argv[1]);
    } else {
        AnalyzeUsage();
        return EXIT_FAILURE;
    }
}