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

bool Buffer_IsValid(struct Buffer* buffer) {
    return (buffer != NULL) && (buffer->data != NULL) && (buffer->length != 0);
}

struct Buffer* Buffer_Clone(struct Buffer* buf) {
    if (!Buffer_IsValid(buf)) { return NULL; }

    struct Buffer* newbuf = Buffer_New(buf->length);
    if (newbuf == NULL) {
        perror("Buffer_Clone");
        return NULL;
    }

    memcpy(newbuf->data, buf->data, buf->length);
    return newbuf;
}
