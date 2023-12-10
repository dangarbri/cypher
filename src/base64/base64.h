#ifndef _BASE64_H_
#define _BASE64_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Encodes length bytes of data into a base64 string
 * @param data data buffer
 * @param length number of bytes to encode
 * @return allocated base64 string. You must free the returned string when you're done.
 */
char* Base64_Encode(uint8_t* data, size_t length);

/**
 * Decoded base64 data
 */
struct Base64Data {
    /** Pointer to decoded data */
    uint8_t* data;
    /** Length of data buffer */
    size_t length;
    /** Validity flag */
    bool valid;
};

/**
 * Decodes the given base64 string
 * @return Info containing decoded data. You must free the returned buffer.
 */
struct Base64Data Base64_Decode(char* data);

/**
 * Base64 subcommand
 */
int Base64(char** argv);

#endif