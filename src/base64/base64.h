#ifndef _BASE64_H_
#define _BASE64_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Base64 data
 * data will be encoded or decoded data depending on which function was returned.
 */
struct Base64Data {
    /** Pointer to data */
    uint8_t* data;
    /** Length of data buffer */
    size_t length;
    /** Validity flag */
    bool valid;
};

/**
 * Encodes length bytes of data into a base64 string
 * @param data data buffer
 * @param length number of bytes to encode
 * @return allocated base64 string. You must free the returned string when you're done.
 */
struct Base64Data Base64_Encode(uint8_t* data, size_t length);

/**
 * Decodes the given base64 string
 * @return Info containing decoded data. You must free the returned buffer.
 */
struct Base64Data Base64_Decode(char* data);

#endif