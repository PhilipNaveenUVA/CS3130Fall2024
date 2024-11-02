#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdint.h> // For uintptr_t
#include <stdalign.h> // For alignas

// Define MAP_ANONYMOUS if not already defined
#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS 0x20 // This value may vary; check documentation for your platform
#endif

// Global array with a specified alignment
alignas(4096) volatile char global_array[4096 * 32]; // Array aligned to 4096 bytes

void labStuff(int which) {
    if (which == 0) {
        printf("No operation performed (which = 0).\n");
    } else if (which == 1) {
        // Write '1' to different pages of the global_array
        for (int i = 0; i < 32; i++) {
            global_array[i * 4096] = '1';  // Writing '1' to each page
        }
        printf("Wrote '1' to each page of global_array.\n");
    } else if (which == 2) {
        char *ptr = malloc(1024 * 1024); // 1024 * 1024 = 1 MB
        if (ptr) {
            // Access a few bytes of the allocation
            ptr[0] = '2'; // Access the first byte
            ptr[1024 * 1024 - 1] = '3'; // Access the last byte
            printf("Allocated 1 MB, set first byte to '2' and last byte to '3'.\n");
            free(ptr); // Remember to free the allocated memory
            printf("Freed the allocated 1 MB memory.\n");
        } else {
            perror("malloc failed");
        }
    } else if (which == 3) {
        // Increase virtual memory size by 1MB and resident set size by 128KB
        size_t page_size = 4096; // Page size
        size_t num_pages = 1048576 / page_size; // 1 MB
        char **pages = malloc(num_pages * sizeof(char*)); // Allocate pointers for each page
        if (pages) {
            for (size_t i = 0; i < num_pages; i++) {
                pages[i] = malloc(page_size); // Allocate each page
                if (pages[i]) {
                    pages[i][0] = '4'; // Access the first byte of each page
                    printf("Allocated page %zu and set first byte to '4'.\n", i);
                } else {
                    perror("malloc failed for page allocation");
                }
            }
            printf("Allocated 1 MB across %zu pages.\n", num_pages);
            // Free the allocated pages
            for (size_t i = 0; i < num_pages; i++) {
                free(pages[i]);
                printf("Freed page %zu.\n", i);
            }
            free(pages);
            printf("Freed all page pointers.\n");
        } else {
            perror("malloc failed for pages");
        }
    } else if (which == 4) {
        // Allocate memory at a specific address
        void *ptr = mmap((void*)0x200000, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
        if (ptr == MAP_FAILED) {
            perror("mmap failed");
        } else {
            memset(ptr, 0, 4096); // Use the allocated memory
            printf("Allocated 4096 bytes at address 0x200000.\n");
            printf("Initialized the allocated memory to zero.\n");
            munmap(ptr, 4096); // Clean up
            printf("Unmapped the allocated memory at 0x200000.\n");
        }
    } else if (which == 5) {
        // Allocate memory further from the heap
        void *ptr = mmap((void*)0x10000000000, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
        if (ptr == MAP_FAILED) {
            perror("mmap failed");
        } else {
            memset(ptr, 0, 4096); // Use the allocated memory
            printf("Allocated 4096 bytes at address 0x10000000000.\n");
            printf("Initialized the allocated memory to zero.\n");
            munmap(ptr, 4096); // Clean up
            printf("Unmapped the allocated memory at 0x10000000000.\n");
        }
    }
}

int main(int argc, char *argv[]) {
    // Check if an argument is passed
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <which>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Convert the argument to an integer
    int which = atoi(argv[1]);

    // Validate the input range
    if (which < 0 || which > 5) {
        fprintf(stderr, "Error: Argument must be between 0 and 5.\n");
        return EXIT_FAILURE;
    }

    printf("Calling labStuff with argument: %d\n", which);
    // Call labStuff with the provided argument
    labStuff(which);
    printf("labStuff completed for argument: %d\n", which);

    return EXIT_SUCCESS;
}

