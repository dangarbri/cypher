#ifndef _BASE64_H_
#define _BASE64_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "types/buffer.h"

/**
 * Encodes length bytes of data into a base64 string
 * @param data data buffer
 * @param length number of bytes to encode
 * @return allocated base64 string. You must free the returned string when you're done.
 */
struct Buffer* Base64_Encode(uint8_t* data, size_t length);

/**
 * Decodes the given base64 string
 * @return Info containing decoded data. You must free the returned buffer.
 */
struct Buffer* Base64_Decode(const char* data);

#endif