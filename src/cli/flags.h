#ifndef _FLAGS_H_
#define _FLAGS_H_

/**
 * Checks if a flag exists in the given input args
 * @returns ptr to flag if it is found, else NULL
 */
char* FlagExists(char* flag, int argc, char** argv);

/**
 * Checks if a specific argument exists in the given input args
 */
#define ArgExists(arg, argc, argv) FlagExists(arg, argc, argv);

#endif