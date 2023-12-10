#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "hex.h"

struct HexData Hex_Parse(char* hex) {
    struct HexData result = {
        .data = NULL,
        .length = 0,
        .valid = false
    };
    size_t hexlen = strlen(hex);
    // Each hex digit must be 2 characters, so the hex
    // string length must be even to be parsable
    if ((hexlen % 2) != 0) {
        return result;
    }
    // Every 2 chars is 1 byte
    size_t bytelen = hexlen / 2;

    // Allocate memory for the data
    uint8_t* data = malloc(bytelen);
    if (data == NULL) {
        perror("Failed to allocate memory for hex parser data");
        return result;
    }

    // Use strtoll to parse long long bytes at a time.
    uint8_t typesize = sizeof(long long);
    uint8_t staging_strlen = typesize * 2;

    // strtoll will return LONG_MAX if the number is too large.
    // Which it will be almost every time, so copy it into a buffer
    // to parse the right number of bytes at a time.
    char* staging_str = malloc(staging_strlen + 1);
    if (data == NULL) {
        perror("Failed to allocate memory for hex parser staging");
        return result;
    }
    // Set null byte on the string
    staging_str[staging_strlen] = 0;
    size_t bytes_copied = 0;
    for (size_t i = 0; i < hexlen; i += staging_strlen) {
        // Copy the next set of data into the staging area
        strncpy(staging_str, &hex[i], staging_strlen);
        // Get the string value as a number
        long long value = strtoll(staging_str, NULL, 16);
        // Copy the data into the output buffer
        size_t bytes_left = bytelen - bytes_copied;
        size_t bytes_to_copy = typesize < bytes_left ? typesize : bytes_left;
        // Copy byte by byte into data
        for (int j = bytes_to_copy; j > 0; j--) {
            long long tmp = value >> ((j-1) * 8);
            data[bytes_copied + bytes_to_copy - j] = tmp & 0xFF;
        }
        bytes_copied += bytes_to_copy;
    }
    free(staging_str);

    result.data = data;
    result.length = bytes_copied;
    result.valid = true;
    return result;
}