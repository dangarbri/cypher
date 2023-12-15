#ifndef _PAD_H_
#define _PAD_H_

#include "types/buffer.h"

/**
 * Interface for a padding algorithm
 * An implementation should pad/unpad the given buffer in-place so that its length is a multiple of blocksize
 * @param buffer data to pad
 * @param blocksize Block size goal
 * @return 0 on success, 1 on failure
 */
typedef int (*PadAlgorithm)(struct Buffer* buffer, size_t blocksize);

/**
 * Interface for an unpadding algorithm
 */
typedef int (*UnpadAlgorithm)(struct Buffer* buffer);

/**
 * Performs pkcs7 padding.
 * @note You should use Pad instead and pass this function as a parameter.
 */
int PKCS7_Pad(struct Buffer* buf, size_t blocksize);

/**
 * Unpads pkcs7 padding text
 * @note Use unpad and pass this function as a parameter
 */
int PKCS7_Unpad(struct Buffer* buf);

#endif