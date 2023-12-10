#ifndef _HEX_H_
#define _HEX_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

struct HexData {
    uint8_t* data;
    size_t length;
    bool valid;
};

/**
 * Parses a hex string int a data buffer
 * @param hex Hex string to parse
 * @return Parsed data buffer. You're responsible for freeing the buffer
 *         If parsing failed, valid bit will be false.
 */
struct HexData Hex_Decode(char* hex);

/**
 * Formats a buffer into an allocated hex string
 * You must free the returned string buffer
 * @param data Data to encode
 * @param length Number of bytes to process
 * @return string buffer or NULL on error
 */
char* Hex_Encode(uint8_t* data, size_t length);

#endif