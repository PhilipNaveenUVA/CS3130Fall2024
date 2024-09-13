#include "split.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Utility function to check if a character is a separator
static int is_separator(char c, const char *sep) {
    return strchr(sep, c) != NULL;
}

char **string_split(const char *input, const char *sep, int *num_words) {
    int count = 0, i = 0, start = 0, end = 0;
    char **result = NULL;
    int len = strlen(input);

    // First pass: Count the number of words (including leading and trailing empty words)
    int word_started = 0;
    while (i <= len) {
        // If we're at a separator, count empty words at the beginning
        if (is_separator(input[i], sep) && !word_started) {
            count++;  // Leading empty word
            word_started = 1;
        }

        // Skip separator characters
        while (i <= len && is_separator(input[i], sep)) i++;

        start = i;

        // Skip non-separator characters (i.e., a word)
        while (i <= len && !is_separator(input[i], sep)) i++;
        end = i;

        // Count the word if it's non-empty or we're at the end
        if (start != end || (i == len && start == len)) {
            count++;
            word_started = 1;
        }
    }

    // If the input ends with a separator, count an additional trailing empty word
    if (is_separator(input[len - 1], sep)) {
        count++;
    }

    // Allocate memory for result array
    result = calloc(count, sizeof(char *));
    if (!result) return NULL;

    i = 0;
    int word_index = 0;

    // Second pass: Extract the words (including empty words)
    word_started = 0;
    while (i <= len) {
        // Handle leading empty word
        if (is_separator(input[i], sep) && !word_started) {
            result[word_index] = malloc(1);  // Empty string
            result[word_index][0] = '\0';
            word_index++;
            word_started = 1;
        }

        // Skip separator characters
        while (i <= len && is_separator(input[i], sep)) i++;
        start = i;

        // Skip non-separator characters (i.e., a word)
        while (i <= len && !is_separator(input[i], sep)) i++;
        end = i;

        // Extract non-empty words
        if (start != end) {
            result[word_index] = malloc(end - start + 1);
            if (!result[word_index]) {
                // Free memory if allocation fails
                for (int j = 0; j < word_index; j++) {
                    free(result[j]);
                }
                free(result);
                return NULL;
            }
            strncpy(result[word_index], input + start, end - start);
            result[word_index][end - start] = '\0';
            word_index++;
        }

        word_started = 1;
    }

    // Handle trailing empty word if the input ends with a separator
    if (is_separator(input[len - 1], sep)) {
        result[word_index] = malloc(1);  // Empty string
        result[word_index][0] = '\0';
        word_index++;
    }

    *num_words = count;
    return result;
}

