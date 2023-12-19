#ifndef _REPLACE_H_
#define _REPLACE_H_

/**
 * Searches the subject for the target and replaces it with replacement
 * @return Updated string, must be free'd. NULL if target is not found
 */
char* streplace(char* subject, char* target, char* replacement);

#endif