#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argtype.h"
#include "parsers/hex.h"
#include "parsers/base64.h"

#ifdef _MSC_VER
#define popen _popen
#define pclose _pclose
#endif

/**
 * Parses hex data into a binary arg
 */
struct Buffer* Argtype_Hex(char* hex) {
    struct Buffer* data = Hex_Decode(hex);
    if (data) {
        return data;
    } else {
        fprintf(stderr, "Unable to parse hex: `%s`\n", hex);
    }
    return NULL;
}

struct Buffer* Argtype_Base64(char* base64) {
    return Base64_Decode(base64);
}

struct Buffer* Argtype_File(char* fname) {
    if (fname == NULL) { return NULL; }

    struct Buffer* arg = NULL;
    bool failure = true;
    FILE* fp = fopen(fname, "rb");
    if (fp != NULL) {
        // Get size of file
        if(fseek(fp, 0, SEEK_END) == 0) {
            long filesize = ftell(fp);
            if (filesize != -1) {
                // Allocate enough bytes to hold the entire file
                // plus 1 to put a null byte at the end.
                // The null byte is there in case the file is a text file.
                // Without terminating the last byte an operation like strlen could
                // read out of bounds.
                arg = Buffer_New(filesize + 1);
                if (Buffer_IsValid(arg)) {
                    // Read in the entire file
                    rewind(fp);
                    size_t bytes_read = fread(arg->data, 1, filesize, fp);
                    if (bytes_read == (arg->length - 1)) {
                        arg->data[bytes_read] = '\0';
                        // Leave the null byte, but say the length is only the length of the data.
                        arg->length -= 1;
                        // Everying is okay!
                        failure = false;
                    }
                }
            }
        } else {
            perror("Error loading file");
        }

        fclose(fp);
    } else {
        fprintf(stderr, "Failed to open file %s: ", fname);
        perror(NULL);
    }

    // On failure, free up allocated resources
    if (failure) {
        Buffer_Free(arg);
        arg = NULL;
    }
    return arg;
}

struct Buffer* Argtype_B64File(char* fname) {
    if (fname == NULL) { return NULL; }
    // Read in the file
    struct Buffer* file = Argtype_File(fname);
    if (file == NULL) {
        return NULL;
    }
    // Base64 decode the file into a new arg
    struct Buffer* decoded = Base64_Decode((char*) file->data);
    Argtype_Free(file);
    return decoded;
}

struct Buffer* Argtype_String(char* arg) {
    // Buffer functions expect this data to be dynamically allocated.
    // i.e. Buffer_Resize attempts to resize this data pointer, so
    // arg cannot be set to the buffer's data
    size_t size = strlen(arg) + 1;
    struct Buffer* result = Buffer_New(size);
    if (Buffer_IsValid(result)) {
        // copy the input string into the buffer and return the result.
        memcpy(result->data, arg, result->length);
        // the null byte is there, but for strings args we only care about the text.
        result->length -= 1;
        return result;
    } else {
        perror("Argtype_String");
    }
    return NULL;
}

struct Buffer* Argtype_Stream(FILE* stream) {
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
struct Buffer* Argtype_Program(char* cmd) {
    FILE* pipe = popen(cmd, "r");
    struct Buffer* buffer = NULL;
    if (pipe) {
        buffer = Argtype_Stream(pipe);
        pclose(pipe);
    }
    return buffer;
}

struct Buffer* Argtype_StreamDecode(FILE* stream, struct Buffer* (*decoder)(char*)) {
    struct Buffer* result = NULL;
    // Process the stream
    struct Buffer* stream_contents = Argtype_Stream(stream);
    if (stream_contents) {
        // Decode the stream's contents
        struct Buffer* decoded = decoder((char*) stream_contents->data);
        // If all is good, assign that to the output
        if (decoded) {
            result = decoded;
        }
        Buffer_Free(stream_contents);
    }
    return result;
}

struct Buffer* Argtype_StreamB64(FILE* stream) {
    return Argtype_StreamDecode(stream, Argtype_Base64);
}

struct Buffer* Argtype_StreamHex(FILE* stream) {
    return Argtype_StreamDecode(stream, Argtype_Hex);
}

struct Buffer* Argtype_New(char* arg) {
    // stdin arguments
    if ((strlen(arg) == 1) && (strcmp(arg, "-") == 0)) {
        return Argtype_Stream(stdin);
    } else if (strcmp(arg, "base64:-") == 0) {
        return Argtype_StreamB64(stdin);
    } else if (strcmp(arg, "hex:-") == 0) {
        return Argtype_StreamHex(stdin);
    }

    char* delim = ":";
    char* type = strtok(arg, delim);
    if (type == NULL) {
        return NULL;
    }
    if (strncmp("hex", type, 3) == 0) {
        char* data = strtok(NULL, delim);
        return Argtype_Hex(data);
    } else if (strncmp("base64", type, 6) == 0) {
        char* data = strtok(NULL, delim);
        return Argtype_Base64(data);
    } else if (strncmp("file.b64", type, 8) == 0) {
        char* fname = strtok(NULL, delim);
        return Argtype_B64File(fname);
    } else if (strncmp("file", type, 4) == 0) {
        char* fname = strtok(NULL, delim);
        return Argtype_File(fname);
    } else if (strncmp("prog", type, 4) == 0) {
        char* command = strtok(NULL, delim);
        return Argtype_Program(command);
    } else {
        return Argtype_String(arg);
    }
}

void Argtype_Free(struct Buffer* arg) {
    Buffer_Free(arg);
}