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
struct Buffer* Buffer_Clone(const struct Buffer* buf);

/**
 * Returns a new buffer, which contains the contents of buf transposed
 * Transposed means that each row of block_size becomes a column of block_size.
 * For example, if buf has 123456789 and you give block_size 3, then the result is 147258369
 * If the transposition can't be done perfectly, the final block is dropped
 * For example if buf has 123456789 and block_size is 4, the result is 15263748 and 9 is dropped
 * @param buf Buffer to transpose
 * @param block_size Block size. Buffer length must be a multiple of block size
 * @returns transposed buffer or NULL
 */
struct Buffer* Buffer_Transpose(const struct Buffer* buf, size_t block_size);

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
bool Buffer_IsValid(const struct Buffer* buffer);

/**
 * Resizes a given buffer and updates its length.
 * @note If this returns false, the buffer remains untouched.
 * @param buffer Buffer to resize
 * @param new_size Desired byte size
 * @returns true on success, false on failure
 */
bool Buffer_Resize(struct Buffer* buffer, size_t new_size);

#endif