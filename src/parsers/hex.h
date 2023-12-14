#ifndef _HEX_H_
#define _HEX_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "types/buffer.h"

/**
 * Parses a hex string int a data buffer
 * @param hex Hex string to parse
 * @return Parsed data buffer. release with Buffer_Free, NULL on failure
 */
struct Buffer* Hex_Decode(char* hex);

/**
 * Formats a buffer into an allocated hex string
 * You must free the returned string buffer
 * @param data Buffer storing data to encode
 * @return string buffer or NULL on error
 */
char* Hex_Encode(struct Buffer* data);

#endif