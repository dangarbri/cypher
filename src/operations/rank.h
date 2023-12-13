#ifndef _RANK_H_
#define _RANK_H_

#include <stddef.h>

/**
 * Ranks the top N integers from the given unsorted array and stores the result in out fields.
 *
 * Example 1: array = [6, 42, 3, 9, 1] and n = 1 will return [1] because index 1 has the max value
 * Example 2: array = [6, 42, 3, 9, 1] and n = 3 will return [1, 3, 0] to represent the top 3 values 42, 9, and 6
 *
 * @param out Array of indices for the top N values. Must be of length n.
 * @param n Number of values to rank
 * @param array Unsorted array of ints to analyze
 * @param length array length
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
int RankInt(size_t* out, size_t n, const int* array, size_t length);

/**
 * Ranks the top N floats from the given unsorted array and stores the result in out fields.
 *
 * Example 1: array = [6, 42, 3, 9, 1] and n = 1 will return [1] because index 1 has the max value
 * Example 2: array = [6, 42, 3, 9, 1] and n = 3 will return [1, 3, 0] to represent the top 3 values 42, 9, and 6
 *
 * @param out Array of indices of array for the top N values. Must be of length n.
 * @param n Number of values to rank
 * @param array Unsorted array of ints to analyze
 * @param length array length
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
int RankFloat(size_t* out, size_t n, const float* array, size_t length);

#endif