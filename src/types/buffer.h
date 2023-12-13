#ifndef _APP_TYPES_H_
#define _APP_TYPES_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Stores buffer information that is commonly used together
 */
struct Buffer {
    /** Pointer to underlying buffer */
    uint8_t* data;
    /** Length of buffer */
    size_t length;
};

/**
 * Allocate a new buffer
 * @param size Number of bytes to allocate
 * @return allocated buffer or NULL on failure
 */
struct Buffer* Buffer_New(size_t size);

/**
 * Allocates a new buffer and copies the contents of buf into it.
 * @param buf buffer to clone
 * @returns new buffer or NULL
 */
struct Buffer* Buffer_Clone(struct Buffer* buf);

/**
 * Frees a buffer allocated with Buffer_New
 * If you wish to claim the underlying buffer, you may set
 * buffer->data to NULL, then call Buffer_Free and it will
 * free the structure, but will not free the data buffer.
 *
 * @param buffer Buffer to free
 */
void Buffer_Free(struct Buffer* buffer);

/**
 * Checks if a buffer is valid and safe to use
 */
bool Buffer_IsValid(struct Buffer* buffer);

#endif