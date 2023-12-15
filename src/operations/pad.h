#ifndef _PAD_H_
#define _PAD_H_

#include "types/buffer.h"

/**
 * Performs pkcs7 padding.
 * @return 1 on success. 0 on failure.
 */
int PKCS7_Pad(struct Buffer* buf, size_t blocksize);

/**
 * Unpads pkcs7 padding text
 * @return 1 on success. 0 on failure.
 */
int PKCS7_Unpad(struct Buffer* buf);

#endif