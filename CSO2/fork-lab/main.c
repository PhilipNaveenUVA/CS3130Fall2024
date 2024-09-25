// File: main.c
#include <stdio.h>
#include <sys/wait.h>
#include "my_system.c"  // Include the my_system function definition

/**
 * @brief Test the my_system function with different shell commands.
 * 
 * @param argc Argument count (not used)
 * @param argv Argument vector (not used)
 * 
 * @return 0 on successful execution of the test program
 */
int main(int argc, const char *argv[]) {
    // First test case: execute three commands and print their exit statuses
    // Command 1: sleep for 1 second, then echo "hi"
    int a1 = my_system("sleep 1; echo hi");
    
    // Command 2: echo "bye"
    int a2 = my_system("echo bye");
    
    // Command 3: try to execute an invalid command "flibbertigibbet 23"
    int a3 = my_system("flibbertigibbet 23");

    // Print the exit statuses of the commands
    // WEXITSTATUS extracts the exit status from the status returned by waitpid
    printf("%d %d %d\n", 
           WEXITSTATUS(a1), WEXITSTATUS(a2), WEXITSTATUS(a3));
    
    // Second test case: prompt the user for input, then echo back the input
    // The command:
    // 1. Prompt the user to "type something"
    // 2. Read the input using `read`
    // 3. Echo back the input inside quotes
    my_system("echo -n 'type something: ';"
              " read got;"
              " echo Thanks for typing \\\"\"$got\"\\\"");
    
    return 0;
}

