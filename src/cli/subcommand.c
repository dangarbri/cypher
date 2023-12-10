#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "subcommand.h"

/**
 * Finds the given subcommand by name
 * @param selector Name of subcommand to search for
 * @param commands List of subcommands to search through, terminated by a null ptr.
 * @returns pointer to found subcommand, or NULL if it couldn't be found
 */
struct Subcommand* Subcommand_Find(char* selector, struct Subcommand* commands[]) {
    struct Subcommand* subcmd = commands[0];
    while ((subcmd != NULL) && (subcmd->name != NULL)) {
        // subcmd name is trusted, so use it as the length limiter.
        // selector is untrusted.
        size_t cmd_name_len = strlen(subcmd->name);
        if (strncmp(subcmd->name, selector, cmd_name_len) == 0) {
            return subcmd;
        }
        subcmd += 1;
    }
    return NULL;
}

/**
 * Returns the length of the longest command name
 */
size_t Subcommand_GetLongestNameLength(struct Subcommand* commands[]) {
    struct Subcommand* subcmd = commands[0];
    size_t longest = 0;
    while ((subcmd != NULL) && (subcmd->name != NULL)) {
        size_t length = strlen(subcmd->name);
        if (length > longest) {
            longest = length;
        }
        subcmd += 1;
    }
    return longest;
}

int Subcommand_Run(char* selector, struct Subcommand commands[], char* argv[]) {
    struct Subcommand* cmd = Subcommand_Find(selector, &commands);
    if (cmd == NULL) {
        return SUBCOMMAND_NOT_FOUND;
    }
    return cmd->fn(argv);
}

void Subcommand_PrintHelp(struct Subcommand commands[]) {
    struct Subcommand* subcmd = &commands[0];
    size_t column_length = Subcommand_GetLongestNameLength(&commands) + 4;
    char format_string[20] = {0};
    snprintf(format_string, 20, "  %%-%lus %%s\n", column_length);
    while ((subcmd != NULL) && (subcmd->name != NULL)) {
        printf(format_string, subcmd->name, subcmd->help);
        subcmd += 1;
    }
}