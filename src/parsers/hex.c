#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "hex.h"

struct Buffer* Hex_Decode(char* hex) {
    if (hex == NULL) { return NULL; }
    size_t hexlen = strlen(hex);
    // Each hex digit must be 2 characters, so the hex
    // string length must be even to be parsable
    if ((hexlen % 2) != 0) {
        return NULL;
    }
    // Every 2 chars is 1 byte
    size_t bytelen = hexlen / 2;
    bool failure = true;
    struct Buffer* result = Buffer_New(bytelen);
    if (result == NULL) {
        return NULL;
    }

    // Use strtoll to parse long long bytes at a time.
    uint8_t typesize = sizeof(long long);
    uint8_t staging_strlen = typesize * 2;

    // strtoll will return LONG_MAX if the number is too large.
    // Which it will be almost every time, so copy it into a buffer
    // to parse the right number of bytes at a time.
    char* staging_str = malloc(staging_strlen + 1);
    if (staging_str != NULL) {
        // Set null byte on the string
        staging_str[staging_strlen] = 0;
        size_t bytes_copied = 0;
        for (size_t i = 0; i < hexlen; i += staging_strlen) {
            // Copy the next set of data into the staging area
            strncpy(staging_str, &hex[i], staging_strlen);
            // Get the string value as a number
            char* endptr = NULL;
            long long value = strtoll(staging_str, &endptr, 16);
            // String is invalid!
            if (*endptr != '\0') {
                fprintf(stderr, "Invalid value %s in hex string", endptr);
                break;
            }
            // Copy the data into the output buffer
            size_t bytes_left = bytelen - bytes_copied;
            size_t bytes_to_copy = typesize < bytes_left ? typesize : bytes_left;
            // Copy byte by byte into data
            for (size_t j = bytes_to_copy; j > 0; j--) {
                long long tmp = value >> ((j-1) * 8);
                result->data[bytes_copied + bytes_to_copy - j] = tmp & 0xFF;
            }
            bytes_copied += bytes_to_copy;
        }
        if (bytes_copied == bytelen) {
            failure = false;
        }
        free(staging_str);
    } else {
        perror("Failed to allocate memory for hex parser staging");
    }

    if (failure) {
        Buffer_Free(result);
        result = NULL;
    }

    return result;
}

char* Hex_Encode(struct Buffer* buffer) {
    if (buffer == NULL) { return NULL; }
    // Each byte is 2 characters
    // +1 for the null terminator
    size_t buflen = (buffer->length * 2) + 1;
    char* out = malloc(buflen);
    if (out != NULL) {
        for (size_t i = 0; i < buffer->length; i++) {
            snprintf(&out[i*2], 3, "%02x", buffer->data[i]);
        }
        out[buflen - 1] = '\0';
        return out;
    } else {
        perror("Hex_Encode: ");
        return NULL;
    }
}
