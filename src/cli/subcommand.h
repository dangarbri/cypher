#ifndef _SUBCOMMAND_H_
#define _SUBCOMMAND_H_

/**
 * Function type for a subcommand
 * @note argv[0] is the first argument for the subcommand, it is not the program name.
 * @param argc number of arguments
 * @param argv CLI arguments
 * @return return status
 */
typedef int (*SubcommandFn)(char* argv[]);

/**
 * Contains metadata about a CLI sub command
 */
struct Subcommand {
    /** Name of subcommand */
    char* name;
    /** Brief help message for sub command */
    char* help;
    /** Pointer to subcommand entry point */
    SubcommandFn fn;
};

#define SUBCOMMAND_NOT_FOUND -1

/**
 * Find and run the given subcommand out of the list of available subcommands
 * @param selector Name of the subcommand to run.
 * @param commands List of all available subcommands, terminated by NULL command (char* name = NULL).
 * @param argv NULL terminated list of arguments to pass to the subcommand
 * @returns Return status of the executed subcommand, or SUBCOMMAND_NOT_FOUND (-1) if the subcommand is not found.
 */
int Subcommand_Run(char* selector, struct Subcommand commands[], char* argv[]);

/**
 * Prints the help messages of all given subcommands to stdout
 */
void Subcommand_PrintHelp(struct Subcommand commands[]);

#endif