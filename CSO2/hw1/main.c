#include "split.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to concatenate command line arguments for separator
char *concat_args(int argc, char **argv) {
    int total_len = 0;
    for (int i = 1; i < argc; i++) {
        total_len += strlen(argv[i]);
    }
    
    char *sep = malloc(total_len + 1);
    if (!sep) return NULL;
    
    sep[0] = '\0';
    for (int i = 1; i < argc; i++) {
        strcat(sep, argv[i]);
    }
    
    return sep;
}

int main(int argc, char *argv[]) {
    char *sep;
    
    // Determine separator characters from command line arguments
    if (argc > 1) {
        sep = concat_args(argc, argv);
        if (!sep) {
            fprintf(stderr, "Memory allocation error\n");
            return 1;
        }
    } else {
        sep = strdup(" \t");
        if (!sep) {
            fprintf(stderr, "Memory allocation error\n");
            return 1;
        }
    }

    char input[4001];
    while (fgets(input, sizeof(input), stdin)) {
        // Trim newline
        input[strcspn(input, "\n")] = '\0';
        
        // Exit on single period "."
        if (strcmp(input, ".") == 0) break;

        int num_words = 0;
        char **words = string_split(input, sep, &num_words);

        // Print out the words with brackets
        if (words) {
            for (int i = 0; i < num_words; i++) {
                printf("[%s]", words[i]);
                free(words[i]);
            }
            printf("\n");
            free(words);
        }
    }

    free(sep);
    return 0;
}

