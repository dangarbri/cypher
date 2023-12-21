#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <stdio.h>

#include "types/buffer.h"

/**
 * Reads the given stream until eof and returns the results in a buffer
 * @return stream contents or NULL on failure.
 */
struct Buffer* ReadStream(FILE* stream);

/**
 * Executes the given command in the system's shell and returns the output.
 * @returns null if the command fails, else stdout results.
 */
struct Buffer* System(const char* command);

#endif