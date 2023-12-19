#include <stdio.h>

#include "system.h"


struct Buffer* ReadStream(FILE* stream) {
    size_t blocksize = 1024;
    size_t bufsize = blocksize;
    size_t remaining_space = bufsize;
    size_t bytes_read = 0;
    struct Buffer* buffer = Buffer_New(bufsize);
    if (buffer) {
        bool success = true;
        while(!feof(stream)) {
            // Read up to enough bytes to fill the buffer
            bytes_read += fread(buffer->data + bytes_read, 1, remaining_space, stream);
            remaining_space = bufsize - bytes_read;
            if (remaining_space == 0) {
                size_t extended_size = bufsize + blocksize;
                // check for overflow
                if (extended_size < bufsize) {
                    fputs("Read limit exceeded\n", stderr);
                    success = false;
                    break;
                }
                bufsize = extended_size;
                // Need to increase buffer size
                if (!Buffer_Resize(buffer, bufsize)) {
                    fputs("Unable to increase buffer size\n", stderr);
                    success = false;
                    break;
                }
            }
        }
        if (!success) {
            Buffer_Free(buffer);
            buffer = NULL;
        }
    }
    return buffer;
}

/**
 * Runs the given command string and uses the command's output as the argument
 */
struct Buffer* System(const char* cmd) {
    FILE* pipe = popen(cmd, "r");
    struct Buffer* buffer = NULL;
    if (pipe) {
        buffer = ReadStream(pipe);
        int exit_code = pclose(pipe);
        if (exit_code != 0) {
            Buffer_Free(buffer);
            buffer = NULL;
        }
    }
    return buffer;
}