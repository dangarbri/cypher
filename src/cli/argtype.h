#ifndef _ARGTYPE_H_
#define _ARGTYPE_H_
/**
 * Input arguments can be prefixed with a type that determines how
 * the argument should be processed.
 * By default arguments are strings.
 * If the arg is written as hex:deadbeef then the input argument will
 * become the array of bytes DE AD BE EF.
 */

#include <stdint.h>
#include <stdbool.h>

#include "types/buffer.h"

/**
 * Parses the given argument into an arg struct.
 * Free this data with Argtype_Free
 *
 * If the type is a string, then the returned ptr will be the same as the input.
 * If the type is hex, then arg will be a ptr to the binary data.
 * If the type is base64, then arg will be a ptr to the decoded binary data.
 * @param arg Input argument to check
 * @return parsed arg or NULL on error.
 */
struct Buffer* Argtype_New(char* arg);

/**
 * Free memory allocated with Argtype_New
 */
void Argtype_Free(struct Buffer* arg);

#endif