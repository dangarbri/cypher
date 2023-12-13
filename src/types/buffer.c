#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"

struct Buffer* Buffer_New(size_t size) {
    // Allocate 2 separate buffers.
    // This way they can be free'd independently
    struct Buffer* buf = malloc(sizeof(struct Buffer));
    if (buf != NULL) {
        buf->length = size;
        buf->data = malloc(size);
        // if data is null, then free the parent buffer
        // and return NULL.
        if (buf->data == NULL) {
            perror("Buffer_New - buf->data: ");
            free(buf);
            buf = NULL;
        }
    } else {
        perror("Buffer_New: ");
    }
    return buf;
}

void Buffer_Free(struct Buffer* buffer) {
    if (buffer != NULL) {
        if (buffer->data != NULL) {
            free(buffer->data);
        }
        free(buffer);
    }
}

bool Buffer_IsValid(const struct Buffer* buffer) {
    return (buffer != NULL) && (buffer->data != NULL) && (buffer->length != 0);
}

struct Buffer* Buffer_Clone(const struct Buffer* buf) {
    if (!Buffer_IsValid(buf)) { return NULL; }

    struct Buffer* newbuf = Buffer_New(buf->length);
    if (newbuf == NULL) {
        perror("Buffer_Clone");
        return NULL;
    }

    memcpy(newbuf->data, buf->data, buf->length);
    return newbuf;
}

struct Buffer* Buffer_Transpose(const struct Buffer* buf, size_t block_size) {
    if (!Buffer_IsValid(buf)) { return NULL; }
    if (buf->length < block_size) { return Buffer_Clone(buf); }

    struct Buffer* transposed = NULL;
    // Clone the input buffer, since we need to modify the data to transpose it
    struct Buffer* clone = Buffer_Clone(buf);
    if (clone != NULL) {
        size_t num_blocks = buf->length / block_size;
        // In the case that block size is 1, copy only one block to the output
        if (num_blocks == 1) {
            transposed = Buffer_New(block_size);
            if (transposed != NULL) {
                for (size_t i = 0; i < block_size; i++) {
                    transposed->data[i] = clone->data[i];
                }
            }
        } else {
            // There are multiple blocks available so we can do transposition.
            // Thanks to integer division, num_blocks represents whole blocks
            // Allocate enough space for the blocks, and perform the transposition.
            transposed = Buffer_New(num_blocks * block_size);
            if (transposed != NULL) {
                uint8_t* blocks[num_blocks];
                // One column for each key byte
                uint8_t* transposed_blocks[block_size];
                // Get a pointer to each block
                for (size_t i = 0; i < num_blocks; i++) {
                    blocks[i] = &clone->data[i * block_size];
                }
                // Get a pointer to each transposed block
                for (size_t i = 0; i < block_size; i++) {
                    transposed_blocks[i] = &transposed->data[i * num_blocks];
                }
                // Transpose the blocks into transposed_blocks
                // Move the 1st byte of each block into the first transposed block
                // Move the 2nd byte of each block into the second transposed block
                // etc
                for (size_t i = 0; i < block_size; i++) {
                    uint8_t* column = transposed_blocks[i];
                    for (size_t j = 0; j < num_blocks; j++) {
                        uint8_t* block = blocks[j];
                        column[j] = block[i];
                    }
                }
            }
        }
        Buffer_Free(clone);
    } else {
        Buffer_Free(transposed);
        transposed = NULL;
    }
    return transposed;
}
