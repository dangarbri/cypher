#include <assert.h>
#include <string.h>
#include "../src/cli/subcommand.h"

int beepboop(int argc, char** argv) {
    (void)argc;
    (void)argv;
    return 10;
}

int dummy_subcommand(int argc, char** argv) {
    (void)argc;
    (void)argv;
    char* expected_argv = "hello";
    (void)expected_argv;
    assert(strncmp(expected_argv, argv[0], strlen(expected_argv)) == 0);
    return 99;
}

struct Subcommand commands[] = {
    {"beepboop", "This is the beepboop command", beepboop},
    {"dummy", "dummy subcommand for testing", dummy_subcommand},
    NULL
};

void test_Subcommand_Run() {
    char* args[] = {"hello"};
    int result = Subcommand_Run("dummy", commands, 1, args);
    assert(result == 99);

    result = Subcommand_Run("nonexistent_command", commands, 1, args);
    assert(result == SUBCOMMAND_NOT_FOUND);
}

int main() {
    test_Subcommand_Run();
}