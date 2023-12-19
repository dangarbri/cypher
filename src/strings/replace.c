#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "replace.h"

char* streplace(char* subject, const char* target, const char* replacement) {
    if (!subject || !target || !replacement) {
        return NULL;
    }
    char* target_location = strstr(subject, target);
    if (!target_location) {
        // Target doesn't exist in subject
        fprintf(stderr, "'%s' doesn't exist in '%s'", subject, target);
        return NULL;
    }

    size_t subject_length = strlen(subject);
    size_t target_length = strlen(target);
    size_t replacement_length = strlen(replacement);
    size_t final_buffer_size = (subject_length - target_length + replacement_length) + 1;
    if (final_buffer_size < subject_length) {
        // overflow detected
        return NULL;
    }
    char* outbuf = malloc(final_buffer_size);
    if (!outbuf) {
        perror("streplace");
        return NULL;
    }
    *outbuf = '\0';

    char* ptr;
    // String to replace is at the start of the buffer, so there's no "left" side
    if (target_location != subject) {
        // Copy left side of subject
        ptr = strtok(subject, target);
        strcat(outbuf, ptr);
    }

    // Insert replacement data
    strcat(outbuf, replacement);

    // Copy right side of subject
    ptr = target_location + target_length;
    if (ptr) {
        strcat(outbuf, ptr);
    }

    return outbuf;
}