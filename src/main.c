#include <stdio.h>
#include "cli/subcommand.h"

int print_help(char* argv[]);

struct Subcommand commands[] = {
    {"help", "prints this help message", print_help},
    {NULL}
};

int print_help(char* argv[]) {
    (void) argv; // supress warning
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
        print_help(NULL);
        return 1;
    } else {
        return Subcommand_Run(argv[1], commands, &argv[2]);
    }
}