#include <assert.h>
#include <string.h>
#include "../src/cli/subcommand.h"

int beepboop(char** argv) {
    return 10;
}

int dummy_subcommand(char** argv) {
    char* expected_argv = "hello";
    assert(strncmp(expected_argv, argv[0], strlen(expected_argv)) == 0);
    return 99;
}

struct Subcommand commands[] = {
    {"beepboop", "This is the beepboop command", beepboop},
    {"dummy", "dummy subcommand for testing", dummy_subcommand},
    NULL
};

void test_Subcommand_Run() {
    char* args[] = {"hello", NULL};
    int result = Subcommand_Run("dummy", commands, args);
    assert(result == 99);

    result = Subcommand_Run("nonexistent_command", commands, args);
    assert(result == SUBCOMMAND_NOT_FOUND);
}

int main() {
    test_Subcommand_Run();
}