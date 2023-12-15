#include <stdio.h>
#include <stdbool.h>

#include "pad.h"

#define SUCCESS 1
#define FAILURE 0

int PKCS7_Pad(struct Buffer* buf, size_t blocksize) {
    // return failure if buffer or blocksize are invalid
    if (!Buffer_IsValid(buf) || (blocksize == 0)) { return FAILURE; }
    // If the buffer is already aligned, return success
    if ((buf->length % blocksize) == 0) { return SUCCESS; }

    // length % blocksize will give how many bytes are in the last "block"
    // blocksize - this value will return how many bytes need to be added on.
    size_t bytes_to_pad = blocksize - (buf->length % blocksize);
    if (bytes_to_pad > 0xFF) {
        fprintf(stderr, "Can't pad %zu bytes\n", bytes_to_pad);
        return FAILURE;
    }

    uint8_t pad_byte = (uint8_t) bytes_to_pad;
    size_t desired_length = buf->length + bytes_to_pad;
    size_t last_byte = buf->length;
    if (Buffer_Resize(buf, desired_length)) {
        for (size_t i = last_byte; i < desired_length; i++) {
            buf->data[i] = pad_byte;
        }
        return SUCCESS;
    } else {
        fputs("Failed resize buffer for pkcs7 padding\n", stderr);
        return FAILURE;
    }
}

int PKCS7_Unpad(struct Buffer* buf) {
    if (!Buffer_IsValid(buf)) { return FAILURE; }
    uint8_t padsize = buf->data[buf->length - 1];
    if (padsize == 0) { return FAILURE; }

    // Validate padding
    uint8_t* end = &buf->data[buf->length - padsize];
    while (end < &buf->data[buf->length]) {
        if (*end != padsize) {
            return FAILURE;
        }
        end += 1;
    }

    buf->length -= padsize;
    buf->data[buf->length] = '\0';
    return SUCCESS;
}
