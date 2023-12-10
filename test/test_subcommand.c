#include <assert.h>
#include <string.h>
#include "../src/cli/subcommand.h"

int dummy_subcommand(char** argv) {
    char* expected_argv = "hello";
    assert(strncmp(expected_argv, argv[0], strlen(expected_argv)) == 0);
    return 99;
}

void test_Subcommand_Run() {
    char* args[] = {"hello", NULL};
    struct Subcommand commands[] = {
        {"dummy", "dummy subcommand for testing", dummy_subcommand},
        NULL
    };
    int result = Subcommand_Run("dummy", commands, args);
    assert(result == 99);

    result = Subcommand_Run("nonexistent_command", commands, args);
    assert(result == SUBCOMMAND_NOT_FOUND);
}


int main() {
    test_Subcommand_Run();
}