#include <stddef.h>
#include <string.h>
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

int Subcommand_Run(char* selector, struct Subcommand commands[], char* argv[]) {
    struct Subcommand* cmd = Subcommand_Find(selector, &commands);
    if (cmd == NULL) {
        return SUBCOMMAND_NOT_FOUND;
    }
    return cmd->fn(argv);
}
