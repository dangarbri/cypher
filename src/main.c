#include <stdio.h>
#include "cli/subcommand.h"
#include "commands/base64_cli.h"

int print_help(int argc, char* argv[]);

struct Subcommand commands[] = {
    {"help", "prints this help message", print_help},
    {"base64", "base64 encoding utilities", Base64},
    {NULL}
};

int print_help(int argc, char* argv[]) {
    // supress warnings
    (void) argc;
    (void) argv;
    puts("Welcome to cypher, the encryption cracking toolkit");
    puts("This program may be used to exploit cryptographic");
    puts("deficiencies as demonstrated in the cryptopals challenges");
    puts("");
    puts("COMMANDS:");
    Subcommand_PrintHelp(commands);
    return 0;
}

int main(int argc, char** argv) {
    (void) argv;
    if (argc <= 1) {
        print_help(0, NULL);
        return 1;
    } else {
        return Subcommand_Run(argv[1], commands, argc - 1, &argv[1]);
    }
}