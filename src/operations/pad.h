#ifndef _PAD_H_
#define _PAD_H_

#include "types/buffer.h"

/**
 * Interface for a padding algorithm
 * An implementation should pad the given buffer in-place so that its length is a multiple of blocksize
 * @param buffer data to pad
 * @param blocksize Block size goal
 * @return 0 on success, 1 on failure
 */
typedef int (*PadAlgorithm)(struct Buffer* buffer, size_t blocksize);

/**
 * Performs pkcs7 padding.
 * @note You should use Pad instead and pass this function as a parameter.
 */
int Pad_PKCS7(struct Buffer* buf, size_t blocksize);

/**
 * Pad the buffer up to the given blocksize using the provided algorithm.
 * @param buffer data to pad
 * @param blocksize Block size goal
 * @param algo Function ptr to padding implementation. defaults to PKCS7 if NULL.
 * @return 0 on success, 1 on failure
 */
int Pad(struct Buffer* buf, size_t blocksize, PadAlgorithm algo);

#endif