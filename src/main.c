#include <stdio.h>
#include "cli/subcommand.h"
#include "commands/analyze.h"
#include "commands/base64_cli.h"
#include "commands/crack.h"
#include "commands/print.h"
#include "commands/xor_cli.h"

int print_help(int argc, char* argv[]);

struct Subcommand commands[] = {
    {"help", "prints this help message", print_help},
    {"analyze", "analyze ciphertext", AnalyzeCmd},
    {"base64", "base64 encoding utilities", Base64},
    {"crack", "encryption cracking utilities", CrackCli},
    {"print", "Print the input to stdout", PrintInput},
    {"xor", "xor on cli arguments", XorCmd},
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
    puts("A lot of crypto operations operate on binary data, not just ascii plaintext.");
    puts("You can enter binary data in a variety of ways:");
    puts("  - \"Hello, world!\" will be used as-is");
    puts("  - hex:48656c6c6f2c20776f726c6421 will become \"Hello, world!\"");
    puts("  - base64:SGVsbG8sIHdvcmxkIQ== will become \"Hello, world!\"");
    puts("  - file:name_of_file will become the contents of file");
    puts("  - file.b64:name_of_file will become the decoded contents of file");
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