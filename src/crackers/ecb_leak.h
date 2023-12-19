#ifndef _ECB_LEAK_H_
#define _ECB_LEAK_H_

/**
 * Leaks data from a program using ecb encryption
 *
 * The `command` input is a string which contains the special phrase "FUZZ" somewhere in it.
 * For example, the command `ecb_leak {'payload': 'contentsFUZZ'}`
 * will modify FUZZ and leave the rest of the text in-tact.
 * @param command The command to execute which returns encrypted data
 * @param verbose Print information as it goes
 * @returns leaked data or NULL on failure.
 */
struct Buffer* ECBLeak(char* command, bool verbose);

#endif